#include "../../test.h"

go_bandit([]() {
  describe("adiar/zdd/elem.cpp", [&]() {
    shared_levelized_file<zdd::node_t> zdd_F;
    shared_levelized_file<zdd::node_t> zdd_T;

    { // Garbage collect writers to free write-lock
      node_writer nw_F(zdd_F);
      nw_F << node(false);

      node_writer nw_T(zdd_T);
      nw_T << node(true);
    }

    const ptr_uint64 terminal_T = ptr_uint64(true);
    const ptr_uint64 terminal_F = ptr_uint64(false);

    shared_levelized_file<zdd::node_t> zdd_1;
    // { { 0 }, { 1 }, { 0,2 }, { 1,2 } }
    /*
            1     ---- x0
           / \
           2 |    ---- x1
          / \|
          F  3    ---- x2
            / \
            T T
     */

    {
      const node n3 = node(2, node::MAX_ID, terminal_T, terminal_T);
      const node n2 = node(1, node::MAX_ID, terminal_F, n3.uid());
      const node n1 = node(0, node::MAX_ID, n2.uid(), n3.uid());

      node_writer nw(zdd_1);
      nw << n3 << n2 << n1;
    }

    shared_levelized_file<zdd::node_t> zdd_2;
    // { Ø, { 1 }, { 2 }, { 2,3 } }
    /*
             1      ---- x1
            / \
            2 T     ---- x2
           / \
           T 3      ---- x3
            / \
            T T

     */

    {
      const node n3 = node(3, node::MAX_ID, terminal_T, terminal_T);
      const node n2 = node(2, node::MAX_ID, terminal_T, n3.uid());
      const node n1 = node(1, node::MAX_ID, n2.uid(), terminal_T);

      node_writer nw(zdd_2);
      nw << n3 << n2 << n1;
    }

    shared_levelized_file<zdd::node_t> zdd_3;
    // { { 2,4 }, { 0,2 }, { 0,4 } }
    /*
            _1_      ---- x0
           /   \
           2   3     ---- x2
          / \ / \
          F  4  T    ---- x4
            / \
            F T
     */
    {
      const node n4 = node(4, node::MAX_ID,   terminal_F, terminal_T);
      const node n3 = node(2, node::MAX_ID,   n4.uid(), terminal_T);
      const node n2 = node(2, node::MAX_ID-1, terminal_F, n4.uid());
      const node n1 = node(1, node::MAX_ID,   n2.uid(), n3.uid());

      node_writer nw(zdd_3);
      nw << n4 << n3 << n2 << n1;
    }

    shared_levelized_file<zdd::node_t> zdd_4;
    // { {1}, {0,1} }
    /*
            1      ---- x0
           | |
            2      ---- x1
           / \
           F T

     */
    {
      const node n2 = node(1, node::MAX_ID, terminal_F, terminal_T);
      const node n1 = node(0, node::MAX_ID, n2.uid(), n2.uid());

      node_writer nw(zdd_4);
      nw << n2 << n1;
    }

    describe("zdd_minelem", [&]() {
      it("finds no element on Ø", [&]() {
        std::optional<adiar::shared_file<zdd::label_t>> result = zdd_minelem(zdd_F);
        AssertThat(result.has_value(), Is().False());
      });

      it("finds empty set on { Ø }", [&]() {
        std::optional<adiar::shared_file<zdd::label_t>> result = zdd_minelem(zdd_T);
        AssertThat(result.has_value(), Is().True());

         adiar::file_stream<zdd::label_t> ls(result.value());
        AssertThat(ls.can_pull(), Is().False());
      });

      it("finds {1} on [1]", [&]() {
        std::optional<adiar::shared_file<zdd::label_t>> result = zdd_minelem(zdd_1);
        AssertThat(result.has_value(), Is().True());

         adiar::file_stream<zdd::label_t> ls(result.value());

        AssertThat(ls.can_pull(), Is().True());
        AssertThat(ls.pull(), Is().EqualTo(1u));

        AssertThat(ls.can_pull(), Is().False());
      });

      it("finds empty set on [2]", [&]() {
        std::optional<adiar::shared_file<zdd::label_t>> result = zdd_minelem(zdd_2);
        AssertThat(result.has_value(), Is().True());

         adiar::file_stream<zdd::label_t> ls(result.value());
        AssertThat(ls.can_pull(), Is().False());
      });

      it("finds {2,4} on [3]", [&]() {
        std::optional<adiar::shared_file<zdd::label_t>> result = zdd_minelem(zdd_3);
        AssertThat(result.has_value(), Is().True());

         adiar::file_stream<zdd::label_t> ls(result.value());

        AssertThat(ls.can_pull(), Is().True());
        AssertThat(ls.pull(), Is().EqualTo(2u));

        AssertThat(ls.can_pull(), Is().True());
        AssertThat(ls.pull(), Is().EqualTo(4u));

        AssertThat(ls.can_pull(), Is().False());
      });

      it("finds {1} on [4]", [&]() {
        std::optional<adiar::shared_file<zdd::label_t>> result = zdd_minelem(zdd_4);
        AssertThat(result.has_value(), Is().True());

         adiar::file_stream<zdd::label_t> ls(result.value());

        AssertThat(ls.can_pull(), Is().True());
        AssertThat(ls.pull(), Is().EqualTo(1u));

        AssertThat(ls.can_pull(), Is().False());
      });
    });

    describe("zdd_maxelem", [&]() {
      it("finds no element on Ø", [&]() {
        std::optional<adiar::shared_file<zdd::label_t>> result = zdd_maxelem(zdd_F);
        AssertThat(result.has_value(), Is().False());
      });

      it("finds empty set on { Ø }", [&]() {
        std::optional<adiar::shared_file<zdd::label_t>> result = zdd_maxelem(zdd_T);
        AssertThat(result.has_value(), Is().True());

         adiar::file_stream<zdd::label_t> ls(result.value());
        AssertThat(ls.can_pull(), Is().False());
      });

      it("finds {0,2} on [1]", [&]() {
        std::optional<adiar::shared_file<zdd::label_t>> result = zdd_maxelem(zdd_1);
        AssertThat(result.has_value(), Is().True());

         adiar::file_stream<zdd::label_t> ls(result.value());

        AssertThat(ls.can_pull(), Is().True());
        AssertThat(ls.pull(), Is().EqualTo(0u));

        AssertThat(ls.can_pull(), Is().True());
        AssertThat(ls.pull(), Is().EqualTo(2u));

        AssertThat(ls.can_pull(), Is().False());
      });

      it("finds {1} on [2]", [&]() {
        std::optional<adiar::shared_file<zdd::label_t>> result = zdd_maxelem(zdd_2);
        AssertThat(result.has_value(), Is().True());

         adiar::file_stream<zdd::label_t> ls(result.value());

        AssertThat(ls.can_pull(), Is().True());
        AssertThat(ls.pull(), Is().EqualTo(1u));

        AssertThat(ls.can_pull(), Is().False());
      });

      it("finds {0,1} on [4]", [&]() {
        std::optional<adiar::shared_file<zdd::label_t>> result = zdd_maxelem(zdd_4);
        AssertThat(result.has_value(), Is().True());

         adiar::file_stream<zdd::label_t> ls(result.value());

        AssertThat(ls.can_pull(), Is().True());
        AssertThat(ls.pull(), Is().EqualTo(0u));

        AssertThat(ls.can_pull(), Is().True());
        AssertThat(ls.pull(), Is().EqualTo(1u));

        AssertThat(ls.can_pull(), Is().False());
      });
    });
  });
 });
