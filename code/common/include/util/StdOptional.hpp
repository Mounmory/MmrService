/**
 * @file StdOptional.hpp
 * @brief 
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef MMR_UTIL_STD_OPTIONAL_H
#define MMR_UTIL_STD_OPTIONAL_H

#if __cplusplus >= 201703L
#include <optional>
#else
#include <exception>
#include <cassert>
#include <type_traits>
#include <utility>

namespace std 
{
    class nullopt_t {
    };

    constexpr nullopt_t nullopt;

    template <typename T>
    class optional {
    public:
        class no_value_exception : public std::exception {
            virtual const char* what() const noexcept override { return "no value"; }
        };

    public:
        optional()
            : _hasValue(false), _value() {
        }

        optional(const T& value)
            : _hasValue(false), _value() {
            emplace(value);
        }

        optional(T&& value)
            : _hasValue(false), _value() {
            emplace(std::forward<T>(value));
        }

        optional(nullopt_t)
            : _hasValue(false), _value() {
        }

        optional(const optional<T>& o)
            : _hasValue(false), _value() {
            if (o.has_value()) {
                emplace(o.value());
            }
        }

        optional(optional<T>&& o) noexcept(std::is_nothrow_move_constructible<T>::value)
            : _hasValue(false), _value() {
            if (o.has_value()) {
                emplace(std::move(o.value()));
                o.reset();
            }
        }

        std::optional<T>& operator=(const std::optional<T>& o) {
            if (o.has_value()) {
                this->emplace(o.value());
            }
            else {
                this->reset();
            }
            return *this;
        }

        std::optional<T>& operator=(std::optional<T>&& o) noexcept(std::is_nothrow_move_constructible<T>::value) {
            if (o.has_value()) {
                this->emplace(std::move(o.value()));
                o.reset();
            }
            else {
                this->reset();
            }
            return *this;
        }

        template <typename T2, std::enable_if_t<std::is_assignable<T, T2>::value, bool> = true>
        std::optional<T>& operator=(T2&& o) {
            this->emplace(std::forward<T2>(o));
            return *this;
        }

        template <typename T2, std::enable_if_t<std::is_assignable<T, T2>::value, bool> = true>
        std::optional<T>& operator=(const T2& o) {
            this->emplace(o);
            return *this;
        }

        ~optional() {
            reset();
        }

        operator bool() const {
            return _hasValue;
        }

        bool has_value() const {
            return _hasValue;
        }

        T& value() {
            if (!_hasValue) {
                assert(!"no value");
                throw no_value_exception();
            }
            return reinterpret_cast<T&>(_value[0]);
        }

        const T& value() const {
            if (!_hasValue) {
                assert(!"no value");
                throw no_value_exception();
            }
            return reinterpret_cast<const T&>(_value[0]);
        }

		const T& operator*() &
		{
			return value();
		}

        template <typename... TArgs>
        T& emplace(TArgs&&... args) {
            if (_hasValue) {
                reset();
            }
            new(_value) T(std::forward<TArgs>(args)...);
            _hasValue = true;
            return value();
        }

        void reset() {
            if (_hasValue) {
                _hasValue = false;
                reinterpret_cast<T&>(_value[0]).~T();
            }
        }

    private:
        static constexpr std::size_t data_size = sizeof(T);
        static constexpr std::size_t align_size = alignof(T);
        std::aligned_storage_t<data_size, align_size> _value[1];
        bool _hasValue;
    };
}
#endif

#endif //MMR_UTIL_STD_OPTIONAL_H





