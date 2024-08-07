#ifndef M1SAKALIB_INDEX_SEQUENCE_H
#define M1SAKALIB_INDEX_SEQUENCE_H

#include <cstddef>

namespace M1sakalib {
    template<std::size_t...index>
    struct index_sequence {};

    template<std::size_t N, std::size_t...index>
    struct make_index_sequence_impl : make_index_sequence_impl<N - 1, N - 1, index...> {};

    template<std::size_t...index>
    struct make_index_sequence_impl<0, index...> {
        using type = index_sequence<index...>;
    };

    template<std::size_t N>
    using make_index_sequence = make_index_sequence_impl<N>::type;
}

#endif //M1SAKALIB_INDEX_SEQUENCE_H
