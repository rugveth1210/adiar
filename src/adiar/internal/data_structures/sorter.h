#ifndef ADIAR_INTERNAL_DATA_STRUCTURES_SORTER_H
#define ADIAR_INTERNAL_DATA_STRUCTURES_SORTER_H

#include <string>
#include <math.h>
#include <algorithm>
#include <memory>

#include <tpie/tpie.h>
#include <tpie/sort.h>

#include <adiar/memory_mode.h>
#include <adiar/internal/assert.h>
#include <adiar/internal/memory.h>

namespace adiar::internal
{
  template <memory_mode_t mem_mode, typename elem_t, typename comp_t = std::less<elem_t>>
  class sorter;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Wrapper for TPIE's internal vector with standard quick-sort.
  //////////////////////////////////////////////////////////////////////////////
  template <typename T, typename pred_t>
  class sorter<memory_mode_t::INTERNAL, T, pred_t>
  {
  private:
    tpie::array<T> _array;
    pred_t _pred;
    size_t _size;
    size_t _front_idx;

  public:
    static tpie::memory_size_type
    unsafe_memory_usage(tpie::memory_size_type no_elements)
    {
      return tpie::array<T>::memory_usage(no_elements);
    }

    static tpie::memory_size_type
    memory_usage(tpie::memory_size_type no_elements)
    {
      const tpie::memory_size_type max_value = std::numeric_limits<tpie::memory_size_type>::max();
      const tpie::memory_size_type max_elem = memory_fits(max_value);
      if (no_elements > max_elem) {
        return max_value;
      }
      return unsafe_memory_usage(no_elements);
    }

    static tpie::memory_size_type
    memory_fits(tpie::memory_size_type memory_bytes)
    {
      const tpie::memory_size_type ret = tpie::array<T>::memory_fits(memory_bytes);
      adiar_assert(unsafe_memory_usage(ret) <= memory_bytes,
                   "memory_fits and memory_usage should agree.");
      return ret;
    }

    static constexpr size_t DATA_STRUCTURES = 1u;

    static unique_ptr<sorter<memory_mode_t::INTERNAL, T, pred_t>>
    make_unique(size_t memory_bytes, size_t no_elements, size_t no_sorters, pred_t pred = pred_t())
    {
      return adiar::make_unique<sorter<memory_mode_t::INTERNAL, T, pred_t>>(memory_bytes, no_elements, no_sorters, pred);
    }

    static void
    reset_unique(unique_ptr<sorter<memory_mode_t::INTERNAL, T, pred_t>> &u_ptr,
                 size_t /*memory_bytes*/,
                 size_t /*no_elements*/,
                 size_t /*no_sorters*/,
                 pred_t /*pred*/ = pred_t())
    {
      u_ptr->reset();
    }

  public:
    sorter([[maybe_unused]] size_t memory_bytes,
           size_t no_elements,
           [[maybe_unused]] size_t no_sorters,
           pred_t pred = pred_t())
      : _array(no_elements), _pred(pred), _size(0), _front_idx(0)
    {
      adiar_debug(no_elements <= memory_fits(memory_bytes / no_sorters),
                  "Must be instantiated with enough memory.");
    }

    void push(const T& t)
    {
      adiar_debug(_array.size() > _size, "Array in internal sorter is too small");
      _array[_size++] = t;
    }

    void sort()
    {
      tpie::parallel_sort(_array.begin(), _array.begin() + _size, _pred);
      _front_idx = 0;
    }

    bool can_pull()
    {
      return _size != _front_idx;
    }

    T pull()
    {
      return _array[_front_idx++];
    }

    void reset()
    {
      _size = 0;
      _front_idx = 0;
    }
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Type alias for sorter for partial type application of the
  ///        'internal' memory type.
  //////////////////////////////////////////////////////////////////////////////
  template <typename elem_t, typename comp_t = std::less<elem_t>>
  using internal_sorter = sorter<memory_mode_t::INTERNAL, elem_t, comp_t>;

  // LCOV_EXCL_START
  // TODO: Unit test external memory variants?

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Wrapper for TPIE's external memory sorter, tpie::merge_sorter.
  ///
  /// A wrapper for the tpie::merge_sorter that takes care of all the memory
  /// computations involved in deriving how much memory should be used and how
  /// it should be.
  ///
  /// TODO:
  /// - Peek function
  //////////////////////////////////////////////////////////////////////////////
  template <typename T, typename pred_t>
  class sorter<memory_mode_t::EXTERNAL, T, pred_t>
  {
  private:
    tpie::merge_sorter<T, false, pred_t> _sorter;

  public:
    static constexpr size_t DATA_STRUCTURES = 1u;

    static unique_ptr<sorter<memory_mode_t::EXTERNAL, T, pred_t>>
    make_unique(size_t memory_bytes, size_t no_elements, size_t no_sorters, pred_t pred = pred_t())
    {
      return adiar::make_unique<sorter<memory_mode_t::EXTERNAL, T, pred_t>>(memory_bytes, no_elements, no_sorters, pred);
    }

    static void reset_unique(unique_ptr<sorter<memory_mode_t::EXTERNAL, T, pred_t>> &u_ptr,
                             size_t memory_bytes,
                             size_t no_elements,
                             size_t no_sorters,
                             pred_t pred = pred_t())
    {
      u_ptr = make_unique(memory_bytes, no_elements, no_sorters, pred);
    }

  public:
    sorter(size_t memory_bytes, size_t no_elements, size_t number_of_sorters, pred_t pred = pred_t())
      : _sorter(pred)
    {
      adiar_debug(number_of_sorters > 0, "Number of sorters should be positive");
      adiar_debug(number_of_sorters > 1, "TODO: memory calculations for single sorter");

      // -----------------------------------------------------------------------
      // We intend to have the memory divided between all the sorters, such that
      // one can be in phase 2 while everyone else is in phase 1 or 3.
      //
      // | p1 | p1 | p1 |       . . . p2 . . .         |
      //
      // Phase two is the one that makes the most out of a lot of memory. So, we
      // want to have phase 2 have the largest share. For simplicity, we
      // currently have all the p1 sorters have 1/16 of the entire memory.
      //
      // TODO: It would be better to make p2 'exponentially' larger than p1 for
      //       some notion of 'exponentiality'.
      //
      // TODO: phase 1 should be upper bounded by the number of elements
      //       possibly placed in this queue. See Issue #189 on ssoelvsten/adiar
      //       as to why. I would suggest for us to upper bound the 1/16th by
      //       slightly more than the amount of memory necessary to hold all
      //       values simultaneously.

      // -----------------------------------------------------------------------
      // Phase 1 : Push Mergesort base cases
      //
      // Quickfix: Issue https://github.com/thomasmoelhave/tpie/issues/250
      constexpr tpie::memory_size_type minimum_phase1 = sizeof(T) * 128 * 1024 + 5 * 1024 * 1024;

      // Consult the internal sorter to get a bound of how much memory is
      // necessary to sort these elements in internal memory. We don't need to
      // allocate more than a constant of this for the external memory case.
      const tpie::memory_size_type no_elements_memory =
        2 * sorter<memory_mode_t::INTERNAL, T, pred_t>::memory_usage(no_elements);

      // Take up at most 1/(Sorter-1)'th of 1/16th of the total memory. The last
      // sorter is either in the same phase or another phase.
      //
      //                Intendeds shared memory layout of sorters
      //   +--------+-----------------------------------------------------+
      //   | p1 . . | p2/p3 . . . . . . . . . . . . . . . . . . . . . . . |
      //   +--------+-----------------------------------------------------+
      const tpie::memory_size_type maximum_phase1 = (memory_bytes >> 4) / (number_of_sorters - 1);

      const tpie::memory_size_type phase1 =
        std::max(minimum_phase1, std::min(maximum_phase1, no_elements_memory));

      // -----------------------------------------------------------------------
      // Phase 3 : Top-most and final merging of partial lists.
      //
      // Based on internal workings of `tpie::merge_sorter` this should be at
      // least twice the size of the phase 1 memory.
      constexpr tpie::memory_size_type minimum_phase3 = /*2 **/ minimum_phase1;

      const tpie::memory_size_type phase3 = std::max(minimum_phase3, phase1);

      // -----------------------------------------------------------------------
      // Phase 2 : Merge sorted lists until there are few enough for phase 3.
      //
      // Use the remaining sorter memory for this very step. If one is in this
      // phase, then all other sorters are in phase 1.
      const tpie::memory_size_type phase2 =
        memory_bytes - phase1 * (number_of_sorters - 1);

      // -----------------------------------------------------------------------
      // Sanity tests...
      adiar_debug(number_of_sorters * phase1 <= memory_bytes,
                  "Memory is enough to have 'N' pushable sorters.");

      adiar_debug((number_of_sorters-1) * phase1 + phase2 <= memory_bytes,
                  "Memory is enough to have 'N-1' pushable sorters and '1' sorting one.");

      adiar_debug((number_of_sorters-1) * phase1 + phase3 <= memory_bytes,
                  "Memory is enough to have 'N-1' pushable sorters and '1' pullable one.");

      adiar_debug((number_of_sorters-1) * phase1 + phase3 + phase1 <= memory_bytes,
                  "Memory is enough to replace the pullable sorter with an 'Nth' pushable one.");

      // -----------------------------------------------------------------------
      // Set the available memory and start the sorter
      _sorter.set_available_memory(phase1, phase2, phase3);
      _sorter.begin();
    }

    void push(const T& t)
    {
      _sorter.push(t);
    }

    void sort()
    {
      _sorter.end();

      tpie::dummy_progress_indicator dpi { };
      _sorter.calc(dpi);
    }

    bool can_pull()
    {
      return _sorter.can_pull();
    }

    T pull()
    {
      return _sorter.pull();
    }
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Type alias for sorter for partial type application of the
  ///        'external' memory type.
  //////////////////////////////////////////////////////////////////////////////
  template <typename elem_t, typename comp_t = std::less<elem_t>>
  using external_sorter = sorter<memory_mode_t::EXTERNAL, elem_t, comp_t>;

  // LCOV_EXCL_END
}

#endif // ADIAR_INTERNAL_DATA_STRUCTURES_SORTER_H
