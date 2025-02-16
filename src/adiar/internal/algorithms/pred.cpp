#include "pred.h"

#include <adiar/internal/io/levelized_file_stream.h>
#include <adiar/internal/cut.h>
#include <adiar/internal/dd.h>
#include <adiar/internal/util.h>

namespace adiar::internal
{
  stats_t::equality_t stats_equality;

  //////////////////////////////////////////////////////////////////////////////
  // Slow O(sort(N)) I/Os comparison by traversing the product construction and
  // comparing each related pair of nodes.

  //////////////////////////////////////////////////////////////////////////////
  // Check whether more requests were processed on this level than allowed. An
  // isomorphic DAG would not create more requests than the original number of
  // nodes.
  template<bool tv>
  class input_bound_levels
  {
  private:
    level_info_stream<> in_meta_1;

    size_t curr_level_size;
    size_t curr_level_processed;

  public:
    static size_t pq1_upper_bound(const shared_levelized_file<node> &in_1,
                                  const shared_levelized_file<node> &in_2)
    {
      return std::max(in_1->max_2level_cut[cut_type::INTERNAL],
                      in_2->max_2level_cut[cut_type::INTERNAL]);
    }

    static size_t pq2_upper_bound(const shared_levelized_file<node> &in_1,
                                  const shared_levelized_file<node> &in_2)
    {
      return std::max(in_1->max_1level_cut[cut_type::INTERNAL],
                      in_2->max_1level_cut[cut_type::INTERNAL]);
    }

    static constexpr size_t memory_usage()
    {
      return level_info_stream<>::memory_usage();
    }

  public:
    input_bound_levels(const shared_levelized_file<node> &f1,
                       const shared_levelized_file<node> &/*f2*/)
      : in_meta_1(f1),
        curr_level_size(in_meta_1.pull().width()),
        curr_level_processed(1)
    { }

    void next_level(ptr_uint64::label_t /* level */)
    { // Ignore input, since only used with the isomorphism_policy below.
      curr_level_size = in_meta_1.pull().width();
      curr_level_processed = 0;
    }

    bool on_step()
    {
      curr_level_processed++;
      const bool ret_value = curr_level_size < curr_level_processed;
#ifdef ADIAR_STATS
      if (ret_value) { stats_equality.slow_check.exit_on_processed_on_level++; }
#endif
      return ret_value;
    }

    static constexpr bool termination_value = tv;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \pre Precondition:
  ///  - The number of nodes are the same
  ///  - The number of levels are the same
  ///  - The label and size of each level are the same
  ///
  /// TODO (Decision Diagrams with other kinds of pointers):
  ///   template<class dd_policy>
  class isomorphism_policy : public prod2_same_level_merger,
                             public dd_policy<dd, __dd>
  {
  public:
    typedef input_bound_levels<false> level_check_t;

  public:
    static constexpr size_t lookahead_bound()
    {
      return 2u;
    }

  public:
    static bool resolve_terminals(const dd::node_t &v1, const dd::node_t &v2, bool &ret_value)
    {
      ret_value = v1.is_terminal() && v2.is_terminal() && v1.value() == v2.value();
#ifdef ADIAR_STATS
      stats_equality.slow_check.exit_on_root++;
#endif
      return true;
    }

  public:
    static bool resolve_singletons(const dd::node_t &v1, const dd::node_t v2)
    {
#ifdef ADIAR_STATS
      stats_equality.slow_check.exit_on_root++;
#endif
      adiar_debug(v1.label() == v2.label(), "Levels match per the precondition");
      return v1.low() == v2.low() && v1.high() == v2.high();
    }

  public:
    template<typename pq_1_t>
    static bool resolve_request(pq_1_t &pq, dd::ptr_t r1, dd::ptr_t r2)
    {
      // Are they both a terminal (and the same terminal)?
      if (r1.is_terminal() || r2.is_terminal()) {
        if (r1.is_terminal() && r2.is_terminal() && r1.value() == r2.value()) {
          return false;
        } else {
#ifdef ADIAR_STATS
          stats_equality.slow_check.exit_on_children++;
#endif
          return true;
        }
      }

      // Do they NOT point to a node with the same level?
      if (r1.label() != r2.label()) {
#ifdef ADIAR_STATS
        stats_equality.slow_check.exit_on_children++;
#endif
        return true;
      }

      // No violation, so recurse
      pq.push({ {r1,r2}, {} });
      return false;
    }

  public:
    // Since we guarantee to be on the same level, then we merely provide a noop
    // (similar to the bdd_policy) for the cofactor.
    static inline void compute_cofactor([[maybe_unused]] bool on_curr_level, dd::ptr_t &, dd::ptr_t &)
    { adiar_invariant(on_curr_level, "No request have mixed levels"); }

  public:
    static constexpr bool early_return_value = false;
    static constexpr bool no_early_return_value = true;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// Fast 2N/B I/Os comparison by comparing the i'th nodes numerically. This
  /// requires, that the shared_levelized_file<node> is 'canonical' in the
  /// following sense:
  ///
  /// - For each level, the ids are decreasing from MAX_ID in increments of one.
  /// - There are no duplicate nodes.
  /// - Nodes within each level are sorted by the children (e.g. ordered first on
  ///   'high', secondly on 'low').
  ///
  /// \remark See Section 3.3 in 'Efficient Binary Decision Diagram Manipulation
  ///         in External Memory' on arXiv (v2 or newer) for an induction proof
  ///         this is a valid comparison.
  ///
  /// \pre The following are satisfied:
  /// (1) The number of nodes are the same (to simplify the 'while' condition)
  /// (2) Both shared_levelized_file<node>s are 'canonical'.
  /// (3) The negation flags given to both shared_levelized_file<node>s agree
  ///     (breaks canonicity)
  //////////////////////////////////////////////////////////////////////////////
  bool fast_isomorphism_check(const shared_levelized_file<node> &f1,
                              const shared_levelized_file<node> &f2)
  {
    node_stream<> in_nodes_1(f1);
    node_stream<> in_nodes_2(f2);

    while (in_nodes_1.can_pull()) {
      adiar_debug(in_nodes_2.can_pull(), "The number of nodes should coincide");
      if (in_nodes_1.pull() != in_nodes_2.pull()) {
#ifdef ADIAR_STATS
        stats_equality.fast_check.exit_on_mismatch++;
#endif
        return false;
      }
    }
    return true;
  }

  //////////////////////////////////////////////////////////////////////////////
  bool is_isomorphic(const shared_levelized_file<node> &f1,
                     const shared_levelized_file<node> &f2,
                     bool negate1, bool negate2)
  {
    // Are they literally referring to the same underlying file?
    if (f1 == f2) {
#ifdef ADIAR_STATS
      stats_equality.exit_on_same_file++;
#endif
      return negate1 == negate2;
    }

    // Are they trivially not the same, since they have different number of
    // nodes?
    if (f1->size() != f2->size()) {
#ifdef ADIAR_STATS
      stats_equality.exit_on_nodecount++;
#endif
      return false;
    }

    // Are they trivially not the same, since they have different number of
    // levels?
    if (f1->levels() != f2->levels()) {
#ifdef ADIAR_STATS
      stats_equality.exit_on_varcount++;
#endif
      return false;
    }

    // Are they trivially not the same, since they have different number of
    // terminal arcs?
    if(f1->number_of_terminals[negate1] != f2->number_of_terminals[negate2] ||
       f1->number_of_terminals[!negate1] != f2->number_of_terminals[!negate2]) {
#ifdef ADIAR_STATS
      stats_equality.exit_on_terminalcount++;
#endif
      return false;
    }

    // Are they trivially not the same, since the labels or the size of each
    // level does not match?
    { // Create new scope to garbage collect the two meta_streams early
      level_info_stream<> in_meta_1(f1);
      level_info_stream<> in_meta_2(f2);

      while (in_meta_1.can_pull()) {
        adiar_debug(in_meta_2.can_pull(), "level_info files are same size");
        if (in_meta_1.pull() != in_meta_2.pull()) {
#ifdef ADIAR_STATS
          stats_equality.exit_on_levels_mismatch++;
#endif
          return false;
        }
      }
    }

    // TODO: Use 'fast_isomorphism_check' when there is only one node per level.

    // Compare their content to discern whether there exists an isomorphism
    // between them.
    if (f1->canonical && f2->canonical && negate1 == negate2) {
#ifdef ADIAR_STATS
      stats_equality.fast_check.runs++;
#endif
      return fast_isomorphism_check(f1, f2);
    } else {
#ifdef ADIAR_STATS
      stats_equality.slow_check.runs++;
#endif
      return comparison_check<isomorphism_policy>(f1, f2, negate1, negate2);
    }
  }

  bool is_isomorphic(const dd &a, const dd &b)
  {
    return is_isomorphic(a.file, b.file, a.negate, b.negate);
  }
}
