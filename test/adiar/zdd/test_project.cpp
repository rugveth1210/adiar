#include "../../test.h"

go_bandit([]() {
  describe("adiar/zdd/project.cpp", [&]() {
    //////////////////////
    // Sink cases

    // Ø
    shared_levelized_file<zdd::node_t> zdd_empty;
    {
      node_writer nw(zdd_empty);
      nw << node(false);
    }

    // { Ø }
    shared_levelized_file<zdd::node_t> zdd_null;
    {
      node_writer nw(zdd_null);
      nw << node(true);
    }

    // TODO: Turn 'GreaterThanOrEqualTo' in max 1-level cuts below into an
    // 'EqualTo'.

    it("computes Ø with dom = {2,4,6} [const &]", [&](){
      adiar::shared_file<zdd::label_t> dom;
      {
        label_writer lw(dom);
        lw << 2 << 4 << 6;
      }

      __zdd out = zdd_project(zdd_empty, dom);

      node_test_stream out_nodes(out);
      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(false)));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);
      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(0u));
    });

    it("computes { Ø } with dom = {1,3,5} [&&]", [&](){
      adiar::shared_file<zdd::label_t> dom;
      {
        label_writer lw(dom);
        lw << 1 << 3 << 5;
      }

      __zdd out = zdd_project(zdd(zdd_null), dom);

      node_test_stream out_nodes(out);
      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(true)));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);
      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(1u));
    });

    it("computes with dom = Ø to be Ø for Ø as input [&&]", [&](){
      adiar::shared_file<zdd::label_t> dom;

      __zdd out = zdd_project(zdd(zdd_empty), dom);

      node_test_stream out_nodes(out);
      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(false)));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);
      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(0u));
    });

    it("computes with dom = Ø to be { Ø } for { Ø } as input [const &]", [&](){
      adiar::shared_file<zdd::label_t> dom;

      __zdd out = zdd_project(zdd_null, dom);

      node_test_stream out_nodes(out);
      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(true)));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);
      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(1u));
    });

    //////////////////////
    // Non-terminal edge cases
    ptr_uint64 terminal_F = ptr_uint64(false);
    ptr_uint64 terminal_T = ptr_uint64(true);

    // { Ø, {0}, {1}, {1,2}, {1,3}, {1,3,4} }
    /*
                     1         ---- x0
                    / \
                    2 T        ---- x1
                   ||
                    3          ---- x2
                   / \
                   4 T         ---- x3
                  / \
                  F 5          ---- x4
                   / \
                   T T
    */
    shared_levelized_file<zdd::node_t> zdd_1;
    {
      node_writer nw(zdd_1);
      nw << node(4, node::MAX_ID, terminal_T, terminal_T)
         << node(3, node::MAX_ID, terminal_F, ptr_uint64(4, ptr_uint64::MAX_ID))
         << node(2, node::MAX_ID, ptr_uint64(3, ptr_uint64::MAX_ID), terminal_T)
         << node(1, node::MAX_ID, ptr_uint64(2, ptr_uint64::MAX_ID), ptr_uint64(2, ptr_uint64::MAX_ID))
         << node(0, node::MAX_ID, ptr_uint64(1, ptr_uint64::MAX_ID), terminal_T)
        ;
    }

    // { {0}, {2}, {0,3}, {2,4} }
    /*
                    1         ---- x0
                   / \
                   |  \       ---- x1
                   |   \
                   2   |      ---- x2
                  / \  |
                  F |  3      ---- x3
                    | / \
                    4 T T     ---- x4
                   / \
                   T T
    */
    shared_levelized_file<zdd::node_t> zdd_2;
    {
      node_writer nw(zdd_2);
      nw << node(4, node::MAX_ID, terminal_T, terminal_T)
         << node(3, node::MAX_ID, terminal_T, terminal_T)
         << node(2, node::MAX_ID, terminal_F, ptr_uint64(4, ptr_uint64::MAX_ID))
         << node(0, node::MAX_ID, ptr_uint64(2, ptr_uint64::MAX_ID), ptr_uint64(3, ptr_uint64::MAX_ID))
        ;
    }

    // { {0}, {2}, {1,2}, {0,2} }
    /*
                    1      ---- x0
                   / \
                   2 |     ---- x1
                  / \|
                  3  4     ---- x2
                 / \/ \
                 F T  T
    */
    shared_levelized_file<zdd::node_t> zdd_3;
    {
      node_writer nw(zdd_3);
      nw << node(2, node::MAX_ID,   terminal_T, terminal_T)
         << node(2, node::MAX_ID-1, terminal_F, terminal_T)
         << node(1, node::MAX_ID,   ptr_uint64(2, ptr_uint64::MAX_ID-1), ptr_uint64(2, ptr_uint64::MAX_ID))
         << node(0, node::MAX_ID,   ptr_uint64(1, ptr_uint64::MAX_ID), ptr_uint64(2, ptr_uint64::MAX_ID))
        ;
    }

    // { {4}, {0,2}, {0,4}, {2,4}, {0,2,4} }
    /*
                    _1_      ---- x0
                   /   \
                   2   3     ---- x2
                   \\ / \
                     4  5    ---- x4
                    / \/ \
                    F  T T
    */
    shared_levelized_file<zdd::node_t> zdd_4;
    {
      node_writer nw(zdd_4);
      nw << node(4, node::MAX_ID,   terminal_T, terminal_T)
         << node(4, node::MAX_ID-1, terminal_F, terminal_T)
         << node(2, node::MAX_ID,   ptr_uint64(4, ptr_uint64::MAX_ID-1), ptr_uint64(4, ptr_uint64::MAX_ID))
         << node(2, node::MAX_ID-1, ptr_uint64(4, ptr_uint64::MAX_ID-1), ptr_uint64(4, ptr_uint64::MAX_ID-1))
         << node(0, node::MAX_ID,   ptr_uint64(2, ptr_uint64::MAX_ID-1), ptr_uint64(2, ptr_uint64::MAX_ID))
        ;
    }

    it("computes with dom = Ø to be { Ø } for non-empty input [zdd_1] [const &]", [&](){
      adiar::shared_file<zdd::label_t> dom;

      __zdd out = zdd_project(zdd_1, dom);

      node_test_stream out_nodes(out);
      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(true)));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);
      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(1u));
    });

    it("computes with dom = Ø to be { Ø } for non-empty input [zdd_2] [&&]", [&](){
      adiar::shared_file<zdd::label_t> dom;

      __zdd out = zdd_project(zdd(zdd_2), dom);

      node_test_stream out_nodes(out);
      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(true)));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);
      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(1u));
    });

    it("computes with dom = Ø to be { Ø } for non-empty input [zdd_3] [const &]", [&](){
      adiar::shared_file<zdd::label_t> dom;

      __zdd out = zdd_project(zdd_3, dom);

      node_test_stream out_nodes(out);
      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(true)));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);
      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(1u));
    });

    it("computes with disjoint dom to be { Ø } [zdd_2] [const &]", [&](){
      adiar::shared_file<zdd::label_t> dom;
      { label_writer lw(dom);
        lw << 1;
      }

      __zdd out = zdd_project(zdd_2, dom);

      node_test_stream out_nodes(out);
      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(true)));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);
      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(1u));
    });

    it("computes with disjoint dom to be { Ø } [zdd_3] [&&]", [&](){
      adiar::shared_file<zdd::label_t> dom;
      { label_writer lw(dom);
        lw << 3 << 4 << 5;
      }

      __zdd out = zdd_project(zdd(zdd_3), dom);

      node_test_stream out_nodes(out);
      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(true)));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);
      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(1u));
    });

    // TODO: Shortcut on nothing to do

    //////////////////////
    // Single-node case
    shared_levelized_file<zdd::node_t> zdd_x1;
    {
      node_writer nw(zdd_x1);
      nw << node(1u, node::MAX_ID, ptr_uint64(false), ptr_uint64(true));
    }

    it("returns { Ø } for {1} with dom = {0} [const &]", [&](){
      adiar::shared_file<zdd::label_t> dom;
      { label_writer lw(dom);
        lw << 0;
      }

      /* Expected: { Ø }
       *
       *        T
       */

      __zdd out = zdd_project(zdd(zdd_x1), dom);

      node_test_stream out_nodes(out);
      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(true)));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);
      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(1u));
    });

    it("returns { 1 } for {1} with dom = {0,1} [const &]", [&](){
      adiar::shared_file<zdd::label_t> dom;
      { label_writer lw(dom);
        lw << 0 << 1;
      }

      /* Expected: { 1 }
       *
       *        1       ---- x1
       *       / \
       *       F T
       */
      __zdd out = zdd_project(zdd(zdd_x1), dom);

      node_test_stream out_nodes(out);
      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(1, node::MAX_ID, zdd::ptr_t(false), zdd::ptr_t(true))));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);
      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(1,1u)));
      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(1u));
    });

    //////////////////////
    // Non-terminal general case
    it("computes zdd_1 with dom = {2,3,4} [const &]", [&](){
      adiar::shared_file<zdd::label_t> dom;
      { label_writer lw(dom);
        lw << 2 << 3 << 4;
      }

      /* Expected: { Ø, {2}, {3}, {3,4} }

                           1    ---- x2
                          / \
                          2 T   ---- x3
                         / \
                         T 3    ---- x4
                          / \
                          T T
      */

      __zdd out = zdd_project(zdd_1, dom);

      node_test_stream out_nodes(out);

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(4, node::MAX_ID,
                                                            terminal_T,
                                                            terminal_T)));

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(3, node::MAX_ID,
                                                            terminal_T,
                                                            ptr_uint64(4, ptr_uint64::MAX_ID))));

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(2, node::MAX_ID,
                                                            ptr_uint64(3, ptr_uint64::MAX_ID),
                                                            terminal_T)));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);

      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(4,1u)));


      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(3,1u)));

      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(2,1u)));

      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(4u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(4u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(4u));
    });

    it("computes zdd_2 with dom = {2,3,4} [&&]", [&](){
      adiar::shared_file<zdd::label_t> dom;
      { label_writer lw(dom);
        lw << 2 << 3 << 4;
      }

      /* Expected: { Ø, {2}, {3}, {2,4} }

                        1      ---- x2
                       / \
                       2  \    ---- x3
                      / \ |
                      T T 3    ---- x4
                         / \
                         T T

      */

      __zdd out = zdd_project(zdd_2, dom);

      node_test_stream out_nodes(out);

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(4, node::MAX_ID,
                                                            terminal_T,
                                                            terminal_T)));

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(3, node::MAX_ID,
                                                            terminal_T,
                                                            terminal_T)));

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(2, node::MAX_ID,
                                                            ptr_uint64(3, ptr_uint64::MAX_ID),
                                                            ptr_uint64(4, ptr_uint64::MAX_ID))));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);

      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(4,1u)));


      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(3,1u)));

      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(2,1u)));

      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(1u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(4u));
    });

    it("computes zdd_3 with dom = {0,2,4}", [&](){
      adiar::shared_file<zdd::label_t> dom;
      { label_writer lw(dom);
        lw << 0 << 2 << 4;
      }

      /* Expected: { {0}, {2}, {0,2} }

                           1    ---- x0
                          / \
                          | |   ---- x1
                          \ /
                           2    ---- x2
                          / \
                          T T
      */

      __zdd out = zdd_project(zdd_3, dom);

      node_test_stream out_nodes(out);

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(2, node::MAX_ID,
                                                            terminal_T,
                                                            terminal_T)));

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(0, node::MAX_ID,
                                                            ptr_uint64(2, ptr_uint64::MAX_ID),
                                                            ptr_uint64(2, ptr_uint64::MAX_ID))));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);

      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(2,1u)));

      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(0,1u)));

      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(2u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(2u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(2u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(2u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(0u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(2u));
    });

    it("computes zdd_4 with dom = {0,4}", [&](){
      adiar::shared_file<zdd::label_t> dom;
      { label_writer lw(dom);
        lw << 0 << 4;
      }

      /* Expected: { {0}, {4}, {0,4} }

                         1     ---- x0
                        / \
                       /   \   ---- x2
                       |   |
                       2   3   ---- x4
                      / \ / \
                      F T T T
      */

      __zdd out = zdd_project(zdd_4, dom);

      node_test_stream out_nodes(out);

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(4, node::MAX_ID,
                                                            terminal_T,
                                                            terminal_T)));

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(4, node::MAX_ID-1,
                                                            terminal_F,
                                                            terminal_T)));

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(0, node::MAX_ID,
                                                            ptr_uint64(4, ptr_uint64::MAX_ID-1),
                                                            ptr_uint64(4, ptr_uint64::MAX_ID))));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);

      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(4,2u)));

      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(0,1u)));

      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(2u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(2u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(3u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(3u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(3u));
    });

    it("computes zdd_4 with dom = {2,4}", [&](){
      adiar::shared_file<zdd::label_t> dom;
      { label_writer lw(dom);
        lw << 2 << 4;
      }

      /* Expected: { {2}, {4}, {2,4} }

                         1     ---- x2
                        / \
                        2 3    ---- x4
                       / \||
                       F  T
      */

      __zdd out = zdd_project(zdd_4, dom);

      node_test_stream out_nodes(out);

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(4, node::MAX_ID,
                                                     terminal_T,
                                                     terminal_T)));

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(4, node::MAX_ID-1,
                                                     terminal_F,
                                                     terminal_T)));

      AssertThat(out_nodes.can_pull(), Is().True());
      AssertThat(out_nodes.pull(), Is().EqualTo(node(2, node::MAX_ID,
                                                     ptr_uint64(4, ptr_uint64::MAX_ID-1),
                                                     ptr_uint64(4, ptr_uint64::MAX_ID))));

      AssertThat(out_nodes.can_pull(), Is().False());

      level_info_test_stream ms(out);

      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(4,2u)));

      AssertThat(ms.can_pull(), Is().True());
      AssertThat(ms.pull(), Is().EqualTo(level_info(2,1u)));

      AssertThat(ms.can_pull(), Is().False());

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL], Is().GreaterThanOrEqualTo(2u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_FALSE], Is().GreaterThanOrEqualTo(2u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::INTERNAL_TRUE], Is().GreaterThanOrEqualTo(2u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->max_1level_cut[cut_type::ALL], Is().GreaterThanOrEqualTo(3u));

      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[false], Is().EqualTo(1u));
      AssertThat(out.get<shared_levelized_file<zdd::node_t>>()->number_of_terminals[true],  Is().EqualTo(3u));
    });
  });
 });
