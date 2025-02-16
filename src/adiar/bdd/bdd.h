#ifndef ADIAR_BDD_BDD_H
#define ADIAR_BDD_BDD_H

#include <string_view>

#include <adiar/internal/io/file.h>
#include <adiar/internal/dd.h>

namespace adiar
{
  class bdd;

  //////////////////////////////////////////////////////////////////////////////
  /// \ingroup module__bdd
  ///
  /// \brief A (possibly) unreduced Binary Decision Diagram.
  ///
  /// \relates bdd
  ///
  /// \copydoc __dd
  //////////////////////////////////////////////////////////////////////////////
  class __bdd : public internal::__dd
  {
  public:
    ////////////////////////////////////////////////////////////////////////////
    __bdd();

    ////////////////////////////////////////////////////////////////////////////
    __bdd(const internal::__dd::shared_nodes_t &f);

    ////////////////////////////////////////////////////////////////////////////
    __bdd(const internal::__dd::shared_arcs_t &f);

    ////////////////////////////////////////////////////////////////////////////
    __bdd(const bdd &bdd);
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \ingroup module__bdd
  ///
  /// \brief A reduced Binary Decision Diagram.
  ///
  /// \extends dd
  ///
  /// \copydoc dd
  //////////////////////////////////////////////////////////////////////////////
  class bdd : public internal::dd
  {
    ////////////////////////////////////////////////////////////////////////////
    // Friends
    // |- classes [public]
    friend __bdd;

    // |- classes [internal]
    friend class apply_prod_policy;

    // |- functions
    friend bdd bdd_not(const bdd&);
    friend bdd bdd_not(bdd&&);
    friend size_t bdd_nodecount(const bdd&);
    friend typename internal::dd::label_t bdd_varcount(const bdd&);

    friend __bdd bdd_ite(const bdd &bdd_if, const bdd &bdd_then, const bdd &bdd_else);

  public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Text to pretty-print in '.dot' output.
    ////////////////////////////////////////////////////////////////////////////
    static constexpr std::string_view false_print = "0";

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Text to pretty-print in '.dot' output.
    ////////////////////////////////////////////////////////////////////////////
    static constexpr std::string_view true_print = "1";

    ////////////////////////////////////////////////////////////////////////////
    // Constructors
  public:
    ////////////////////////////////////////////////////////////////////////////
    bdd();

    ////////////////////////////////////////////////////////////////////////////
    bdd(const internal::dd::shared_nodes_t &f, bool negate = false);

    ////////////////////////////////////////////////////////////////////////////
    bdd(const bdd &o);

    ////////////////////////////////////////////////////////////////////////////
    bdd(bdd &&o);

    ////////////////////////////////////////////////////////////////////////////
    bdd(__bdd &&o);

    ////////////////////////////////////////////////////////////////////////////
    bdd(bool v);

    ////////////////////////////////////////////////////////////////////////////
    // Assignment operator overloadings
  public:
    bdd& operator= (const bdd &other);
    bdd& operator= (__bdd &&other);

    ////////////////////////////////////////////////////////////////////////////
    bdd& operator&= (const bdd &other);
    bdd& operator&= (bdd &&other);

    ////////////////////////////////////////////////////////////////////////////
    bdd& operator|= (const bdd &other);
    bdd& operator|= (bdd &&other);

    ////////////////////////////////////////////////////////////////////////////
    bdd& operator^= (const bdd &other);
    bdd& operator^= (bdd &&other);
  };
}

#endif // ADIAR_BDD_BDD_H
