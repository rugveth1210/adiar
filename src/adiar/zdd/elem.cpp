#include <adiar/zdd.h>

#include <adiar/internal/algorithms/traverse.h>
#include <adiar/internal/data_types/node.h>
#include <adiar/internal/data_types/uid.h>
#include <adiar/internal/io/file.h>
#include <adiar/internal/io/file_stream.h>
#include <adiar/internal/io/file_writer.h>

namespace adiar
{
  template<typename visitor_t>
  class zdd_sat_label_writer_visitor
  {
    visitor_t visitor;

    bool has_elem = false;

    shared_file<zdd::label_t> lf;
    internal::label_writer lw;

  public:
    zdd_sat_label_writer_visitor() : lw(lf) { }

    zdd::ptr_t visit(const zdd::node_t n)
    {
      const zdd::ptr_t next_ptr = visitor.visit(n);

      if (next_ptr == n.high() && (next_ptr != n.low() || visitor_t::keep_dont_cares)) {
        lw << n.label();
      }

      return next_ptr;
    }

    void visit(const bool s)
    {
      visitor.visit(s);
      has_elem = s;
    }

    const std::optional<shared_file<zdd::label_t>> get_result() const
    {
      if (has_elem) { return lf; } else { return std::nullopt; }
    }
  };

  class zdd_satmin_visitor : public internal::traverse_satmin_visitor
  {
  public:
    static constexpr bool keep_dont_cares = false;
  };

  std::optional<shared_file<zdd::label_t>>
  zdd_minelem(const zdd &A)
  {
    zdd_sat_label_writer_visitor<zdd_satmin_visitor> v;
    internal::traverse(A, v);
    return v.get_result();
  }

  class zdd_satmax_visitor
  {
  public:
    static constexpr bool keep_dont_cares = true;

    inline zdd::ptr_t visit(const zdd::node_t n) {
      adiar_debug(!n.high().is_terminal() || n.high().value(), "high terminals are never false");
      return n.high();
    }

    inline void visit(const bool /*s*/)
    { }
  };

  std::optional<shared_file<zdd::label_t>>
  zdd_maxelem(const zdd &A)
  {
    zdd_sat_label_writer_visitor<zdd_satmax_visitor> v;
    internal::traverse(A, v);
    return v.get_result();
  }
}
