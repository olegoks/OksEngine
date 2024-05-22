#pragma once

#include <type_traits>
#include <concepts>
#include <memory>

namespace Memory {

	//using value_type = T;
	//using pointer = T*;
	//using const_pointer = const T*;
	//using reference = T&;
	//using const_reference = const T&;

	//pointer allocate(size_type n);
	//void deallocate(pointer p, size_type n);

	//size_type max_size() const noexcept;
	
	//template<typename U, typename... Args>
	//void construct(U* p, Args&&... args);

	//template<typename U>
	//void destroy(U* p);
	



	template<class Type>
	concept Allocator = requires(Type allocator) {
        allocator.allocate(1);
	};

	//template<typename T>
	//concept Hashable = requires(T a)
	//{
	//	{ std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
	//};

    template<typename T>
    concept copy_assignable = std::is_copy_assignable_v<T>;

    template<typename T>
    concept move_assignable = std::is_move_assignable_v<T>;

    template<typename T>
    concept nullable_pointer =
        std::equality_comparable<T> &&
        std::default_initializable<T> &&
        std::copy_constructible<T> &&
        copy_assignable<T> &&
        std::destructible<T> &&
        requires (T p, T q) {
            { T(nullptr) }   -> std::same_as<T>;
            { p = nullptr }  -> std::same_as<T&>;
            { p != q }       -> std::convertible_to<bool>;
            { p == nullptr } -> std::convertible_to<bool>;
            { nullptr == p } -> std::convertible_to<bool>;
            { p != nullptr } -> std::convertible_to<bool>;
            { nullptr != p } -> std::convertible_to<bool>;
    };

    template<
        typename Alloc,
        typename T = Alloc::value_type,
        typename pointer = std::allocator_traits<Alloc>::pointer,
        typename const_pointer = std::allocator_traits<Alloc>::const_pointer,
        typename void_pointer = std::allocator_traits<Alloc>::void_pointer,
        typename const_void_pointer =
        std::allocator_traits<Alloc>::const_void_pointer,
        typename size_type = std::allocator_traits<Alloc>::size_type,
        typename difference_type = std::allocator_traits<Alloc>::difference_type
    >
    concept AllocatorType =
        nullable_pointer<pointer> &&
        std::random_access_iterator<pointer> &&
        std::contiguous_iterator<pointer> &&
        nullable_pointer<const_pointer> &&
        std::random_access_iterator<const_pointer> &&
        std::contiguous_iterator<const_pointer> &&
        std::convertible_to<pointer, const_pointer> &&
        nullable_pointer<void_pointer> &&
        std::convertible_to<pointer, void_pointer> &&
        nullable_pointer<const_void_pointer> &&
        std::convertible_to<pointer, const_void_pointer> &&
        std::convertible_to<const_pointer, const_void_pointer> &&
        std::convertible_to<void_pointer, const_void_pointer> &&
        std::unsigned_integral<size_type> &&
        std::signed_integral<difference_type> &&
        std::copy_constructible<Alloc> &&
        std::move_constructible<Alloc> &&
         copy_assignable<Alloc> &&
         move_assignable<Alloc> &&
        std::equality_comparable<Alloc> &&
        requires(Alloc a) {
        typename T;

        // *p
        {*std::declval<pointer>()} -> std::same_as<T&>;

        // *cp
        {*std::declval<const_pointer>()} -> std::same_as<const T&>;

        // static_cast<Alloc::pointer>(vp)
            requires
        std::same_as<
            decltype(static_cast<T*>(std::declval<void_pointer>())),
            pointer
        >;

        // static_cast<Alloc::const_pointer>(cvp)
            requires
        std::same_as<
            decltype(static_cast<const_pointer>(
                std::declval<const_void_pointer>())),
            const_pointer
        >;

        // std::pointer_traits<Alloc::pointer>::pointer_to(r)
        {std::pointer_traits<pointer>::pointer_to(*std::declval<pointer>())}
        -> std::same_as<pointer>;

        {a.allocate(std::declval<size_type>())} -> std::same_as<pointer>;

        {a.deallocate(std::declval<pointer>(), std::declval<size_type>())}
        -> std::same_as<void>;
    };

}