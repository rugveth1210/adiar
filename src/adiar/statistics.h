#ifndef ADIAR_STATISTICS_H
#define ADIAR_STATISTICS_H

////////////////////////////////////////////////////////////////////////////////
/// \defgroup module__statistics Statistics
///
/// \brief Statistics on the internal algorithms and data structures.
///
/// \pre Statistics are by default **not** gathered due to a concern of the
///      performance of Adiar. That is, the logic related to updating the
///      statistics is only run when the `ADIAR_STATS` CMake variable is set to
///      `ON`.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstddef>

#include <adiar/internal/cnl.h>

namespace adiar
{
  //////////////////////////////////////////////////////////////////////////////
  /// \addtogroup module__statistics
  ///
  /// \{

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Available statistics from algorithm's and data structures.
  ///
  /// \pre These numbers are always available, but they are only populated
  ///      with actual statistics if Adiar is compiled with the `ADIAR_STATS`
  ///      CMake variable set to `ON`.
  //////////////////////////////////////////////////////////////////////////////
  struct stats_t
  {
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Levelized Priority Queue statistics.
    ////////////////////////////////////////////////////////////////////////////
    struct levelized_priority_queue_t
    {
      //////////////////////////////////////////////////////////////////////////
      /// \brief Number of pushes in the *bucketed* variant to a bucket.
      //////////////////////////////////////////////////////////////////////////
      uintwide_t push_bucket = 0;

      //////////////////////////////////////////////////////////////////////////
      /// \brief Number of pushes in the *bucketed* variant to the overflow queue.
      //////////////////////////////////////////////////////////////////////////
      uintwide_t push_overflow = 0;

      //////////////////////////////////////////////////////////////////////////
      /// \brief The sum over all levelized priority queue's predicted maximum
      /// size, i.e. \f$ \sum_{\mathit{pq} \in \mathit{PQ}}
      /// \mathit{pq}.\mathit{predicted\_size} \f$
      ///
      /// \see sum_actual_max_size
      //////////////////////////////////////////////////////////////////////////
      uintwide_t sum_predicted_max_size = 0;

      //////////////////////////////////////////////////////////////////////////
      /// \brief The sum over all levelized priority queue's maximum size, i.e.
      /// \f$ \sum_{\mathit{pq} \in \mathit{PQ}} \mathit{pq}.\mathit{max\_size}
      /// \f$
      ///
      /// \see sum_predicted_max_size
      //////////////////////////////////////////////////////////////////////////
      uintwide_t sum_actual_max_size = 0;

      //////////////////////////////////////////////////////////////////////////
      /// \brief Sum over the ratio between predicted and actual maximum size,
      /// i.e. \f$ \sum_{\mathit{pq} \in \mathit{PQ}}
      /// \frac{\mathit{pq}.\mathit{max\_size}}{\mathit{pq}.\mathit{predicted\_size}}
      /// \f$.
      //////////////////////////////////////////////////////////////////////////
      double sum_max_size_ratio = 0.0;

      //////////////////////////////////////////////////////////////////////////
      /// \brief Number of calls to the destructor, i.e. the total number of
      /// levelized priority queues that have reported their statistics.
      //////////////////////////////////////////////////////////////////////////
      size_t sum_destructors = 0;
    }
    /// \copydoc levelized_priority_queue_t
    levelized_priority_queue;

    /// \}

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Common statistics for all algorithms
    ////////////////////////////////////////////////////////////////////////////
    struct __alg_base
    {
      //////////////////////////////////////////////////////////////////////////
      /// \brief Type and usage of this algorithm's levelized priority queue.
      //////////////////////////////////////////////////////////////////////////
      struct __lpq : public levelized_priority_queue_t
      {
        ////////////////////////////////////////////////////////////////////////
        /// \brief Number of *unbucketed internal* levelized priority queues.
        ////////////////////////////////////////////////////////////////////////
        uintwide_t unbucketed = 0;

        ////////////////////////////////////////////////////////////////////////
        /// \brief Number of *bucketed internal* levelized priority queues.
        ////////////////////////////////////////////////////////////////////////
        uintwide_t internal = 0;

        ////////////////////////////////////////////////////////////////////////
        /// \brief Number of *bucketed external* levelized priority queues.
        ////////////////////////////////////////////////////////////////////////
        uintwide_t external = 0;

        ////////////////////////////////////////////////////////////////////////
        /// \brief Total number of levelized priority queues.
        ////////////////////////////////////////////////////////////////////////
        uintwide_t total() const
        {
          return unbucketed + internal + external;
        }
      }
      /// \copydoc __lpq
      lpq;
    };

    ////////////////////////////////////////////////////////////////////////////
    // Top-down Sweeps

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Counting algorithm statistics.
    ///
    /// \see bdd_satcount bdd_pathcount zdd_size
    ////////////////////////////////////////////////////////////////////////////
    struct count_t : public __alg_base
    { }
    /// \copydoc count_t
    count;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Equality Checking algorithm statistics.
    ///
    /// \see bdd_equal zdd_equal
    ////////////////////////////////////////////////////////////////////////////
    struct equality_t : public __alg_base
    {
      // Early termination cases

      //////////////////////////////////////////////////////////////////////////
      /// \brief Early *O(1)* termination due to same
      ///        \ref shared_levelized_file<...>.
      //////////////////////////////////////////////////////////////////////////
      uintwide_t exit_on_same_file = 0;

      //////////////////////////////////////////////////////////////////////////
      /// \brief Early *O(1)* termination due to mismatch in number of nodes.
      //////////////////////////////////////////////////////////////////////////
      uintwide_t exit_on_nodecount = 0;

      //////////////////////////////////////////////////////////////////////////
      /// \brief Early *O(1)* termination due to mismatch in number of levels.
      //////////////////////////////////////////////////////////////////////////
      uintwide_t exit_on_varcount = 0;

      //////////////////////////////////////////////////////////////////////////
      /// \brief Early *O(1)* termination due to mismatch in number of arcs to
      ///        terminals.
      //////////////////////////////////////////////////////////////////////////
      uintwide_t exit_on_terminalcount = 0;

      //////////////////////////////////////////////////////////////////////////
      /// \brief Early *O(L)* termination due to per-level meta information does
      ///        not match.
      //////////////////////////////////////////////////////////////////////////
      uintwide_t exit_on_levels_mismatch = 0;

      //////////////////////////////////////////////////////////////////////////
      /// \brief Statistics from *O(N log N)* time-forward processing equality
      ///        checking algorithm.
      //////////////////////////////////////////////////////////////////////////
      struct slow_t
      {
        ////////////////////////////////////////////////////////////////////////
        /// \brief Number of runs of the slow isomorphism checking algorithm.
        ////////////////////////////////////////////////////////////////////////
        uintwide_t runs = 0;

        ////////////////////////////////////////////////////////////////////////
        /// \brief Termination due to a local violation at the root.
        ////////////////////////////////////////////////////////////////////////
        uintwide_t exit_on_root = 0;

        ////////////////////////////////////////////////////////////////////////
        /// \brief Termination due to too many requests being processed at some
        ///        level, i.e. one node must have been paired with two or more.
        ////////////////////////////////////////////////////////////////////////
        uintwide_t exit_on_processed_on_level = 0;

        ////////////////////////////////////////////////////////////////////////
        /// \brief Termination due to mismatch in a node's children's level,
        ///        i.e. a local violation at some node.
        ////////////////////////////////////////////////////////////////////////
        uintwide_t exit_on_children = 0;
      }
      /// \copydoc slow_t
      slow_check;

      //////////////////////////////////////////////////////////////////////////
      /// \brief Statistics from *O(N)* linear-scan equality checking algorithm.
      ///
      /// \see   is_canonical
      //////////////////////////////////////////////////////////////////////////
      struct fast_t
      {
        ////////////////////////////////////////////////////////////////////////
        /// \brief Number of runs of the fast isomorphism algorithm.
        ////////////////////////////////////////////////////////////////////////
        uintwide_t runs = 0;

        ////////////////////////////////////////////////////////////////////////
        /// \brief Termination due to the i'th nodes do not match numerically.
        ////////////////////////////////////////////////////////////////////////
        uintwide_t exit_on_mismatch = 0;
      }
      /// \copydoc fast_t
      fast_check;
    }
    /// \copydoc equality_t
    equality;

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Intercut algorithm statistics.
    ///
    /// \see bdd_from zdd_change zdd_complement zdd_expand zdd_from
    //////////////////////////////////////////////////////////////////////////////
    struct intercut_t : public __alg_base
    { }
    /// \copydoc intercut_t
    intercut;

    //////////////////////////////////////////////////////////////////////////////
    /// \brief 2-ary Product Construction algorithm statistics.
    ///
    /// \see bdd_apply zdd_binop
    //////////////////////////////////////////////////////////////////////////////
    struct prod2_t : public __alg_base
    { }
    /// \copydoc prod2_t
    prod2;

    //////////////////////////////////////////////////////////////////////////////
    /// \brief 3-ary Product Construction algorithm statistics.
    ///
    /// \see bdd_ite
    //////////////////////////////////////////////////////////////////////////////
    struct prod3_t : public __alg_base
    { }
    /// \copydoc prod3_t
    prod3;

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Quantification algorithm statistics.
    ///
    /// \see bdd_exists bdd_forall zdd_project
    //////////////////////////////////////////////////////////////////////////////
    struct quantify_t : public __alg_base
    { }
    /// \copydoc quantify_t
    quantify;

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Substitution algorithm statistics.
    ///
    /// \see bdd_restrict zdd_onset zdd_offset
    //////////////////////////////////////////////////////////////////////////////
    struct substitute_t : public __alg_base
    { }
    /// \copydoc substitute_t
    substitute;

    //////////////////////////////////////////////////////////////////////////////
    // Bottom-up Sweeps

    //////////////////////////////////////////////////////////////////////////////
    /// \brief Reduce algorithm statistics.
    ///
    /// \see bdd zdd
    //////////////////////////////////////////////////////////////////////////////
    struct reduce_t : public __alg_base
    {
      ////////////////////////////////////////////////////////////////////////
      /// \brief Sum of the inputs' number of arcs to internal nodes.
      ////////////////////////////////////////////////////////////////////////
      uintwide_t sum_node_arcs = 0;

      ////////////////////////////////////////////////////////////////////////
      /// \brief Sum of the inputs' number of arcs to terminals.
      ////////////////////////////////////////////////////////////////////////
      uintwide_t sum_terminal_arcs = 0;

      ////////////////////////////////////////////////////////////////////////
      /// \brief Number of nodes removed due to reduction rule 1, i.e. the
      ///        number of nodes that are suppressed in the specific type of
      ///        decision diagram.
      ////////////////////////////////////////////////////////////////////////
      uintwide_t removed_by_rule_1 = 0;

      ////////////////////////////////////////////////////////////////////////
      /// \brief Number of nodes removed due to reduction rule 2, i.e. the
      ///        number of duplicate of nodes that have been removed.
      ////////////////////////////////////////////////////////////////////////
      uintwide_t removed_by_rule_2 = 0;
    }
    /// \copydoc reduce_t
    reduce;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Obtain a copy of all statistics gathered.
  ///
  /// \copydoc stats_t
  //////////////////////////////////////////////////////////////////////////////
  stats_t adiar_stats();

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Print statistics to an output stream (default `std::cout`).
  ///
  /// \copydoc stats_t
  //////////////////////////////////////////////////////////////////////////////
  void adiar_printstat(std::ostream &o = std::cout);

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Resets all statistics to default value.
  ///
  /// \see stats_t
  //////////////////////////////////////////////////////////////////////////////
  void adiar_statsreset();

  /// \}
  //////////////////////////////////////////////////////////////////////////////
}

#endif // ADIAR_STATISTICS_H
