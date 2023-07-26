#pragma once

#include <functional>
#include <memory>
#include <type_traits>

namespace utils::lazy
{

/**
 * @brief LazyWrap is a wrapper that calculate or get the value only then, 
 *        when it is realy necessary.

 * @details LazyWrap - lazy evaluation concept.
 * @tparam Type
 */
template<class Type>
class LazyWrap
{
public:
    LazyWrap() = delete;

    LazyWrap(std::function<Type()> getter) 
        : _lazybonesGetter(getter)
    {
    }

    Type getLazybones() const
    {
        return _lazybonesGetter();
    }

private:
    std::function<Type()> _lazybonesGetter;
};

template<class Type>
class ILazy
{
public:
    virtual ~ILazy() = default;

    virtual std::shared_ptr<Type> lazyThis() noexcept = 0;
};

/**
 * @brief enable_lazy_from_this - Class that allows an object currently managed by std::shared_ptr 
 *        to get a "lazy" wrapper on shared_from_this/weak_from_this. 
 *        Conceptually, it is a "lazy" analog of enable_shared_from_this.

 * @tparam Type 
 */
template <class Type>
class enable_lazy_from_this : public std::enable_shared_from_this<Type>
{
private:
    using _ValuePtr = std::shared_ptr<Type>;

    template <class _Type>
    using _TypedLazyWrap = LazyWrap<_Type>;

    template <class _Type>
    using _TypedLazyWrapPtr = std::shared_ptr<_TypedLazyWrap<_Type>>;

public:
    virtual ~enable_lazy_from_this() = default;

    enable_lazy_from_this() {
        _lazybonesGetter = 
            [this]() -> _ValuePtr 
            {
                // "this" can be a pointer to the base class, so we need to do down-cast 
                // because "shared_from_this" by base class can be nullptr.
                const auto __this = dynamic_cast<Type*>(this);
                return __this->shared_from_this();
            };
    }

    _TypedLazyWrapPtr<Type> lazy_from_this() noexcept 
    {
        if (not _cachedLazyWrap)
            _cachedLazyWrap = std::make_shared<_TypedLazyWrap>(_lazybonesGetter);
        return _cachedLazyWrap;
    }

    template<
        class BaseType,
        std::enable_if_t<std::is_base_of_v<BaseType, Type>, bool> = true
    >
    _TypedLazyWrapPtr<BaseType> 
    lazy_from_this() noexcept 
    {
        return lazy_cast<Type, BaseType>(this->lazy_from_this());
    }

private:
    std::function<std::shared_ptr<Type>()> _lazybonesGetter;
    _TypedLazyWrapPtr<Type> _cachedLazyWrap;
};

/**
 * @brief lazy_cast - Converts between base and child class that used "LazyWrap"

 * @tparam Derive 
 * @tparam Base 
 * @param derive 
 * @return std::shared_ptr<LazyWrap<std::shared_ptr<Base>>> 
 */
template<
    class Derive,
    class Base,
    std::enable_if_t<std::is_base_of_v<Base, Derive>, bool> = true
>
std::shared_ptr<LazyWrap<std::shared_ptr<Base>>>
lazy_cast(std::shared_ptr<LazyWrap<std::shared_ptr<Derive>>> derive)
{
    if (not derive)
        return {};
    
    return std::make_shared<LazyWrap<std::shared_ptr<Base>>>(
        [derive]() -> std::shared_ptr<Base> {
            if (not derive or not derive->getLazybones())
                return {};
            
            return std::static_pointer_cast<Base>(derive->getLazybones()->shared_from_this());
        }
    );
}

template<
    class Derive,
    class Base,
    std::enable_if_t<std::is_base_of_v<Base, Derive>, bool> = true
>
std::shared_ptr<LazyWrap<std::weak_ptr<Base>>>
lazy_cast(std::shared_ptr<LazyWrap<std::weak_ptr<Derive>>> derive)
{
    if (not derive)
        return {};
    
    return std::make_shared<LazyWrap<std::weak_ptr<Base>>>(
        [derive]() -> std::weak_ptr<Base> {
            if (not derive or not derive->getLazybones())
                return {};
            
            return std::static_pointer_cast<Base>(derive->getLazybones()->shared_from_this());
        }
    );
}

template<
    class Derive,
    class Base,
    std::enable_if_t<std::is_base_of_v<Base, Derive>, bool> = true
>
LazyWrap<std::shared_ptr<Base>>
lazy_cast(LazyWrap<std::shared_ptr<Derive>> derive)
{
    if (not derive)
        return {};
    
    return LazyWrap<std::shared_ptr<Base>>(
        [derive]() -> std::shared_ptr<Base> {
            if (not derive or not derive->getLazybones())
                return {};
            
            return std::static_pointer_cast<Base>(derive->getLazybones()->shared_from_this());
        }
    );
}

} // namespace utils::lazy
