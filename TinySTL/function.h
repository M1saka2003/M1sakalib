#ifndef M1SAKALIB_FUNCTION_H
#define M1SAKALIB_FUNCTION_H

#include <utility>
#include <memory>
#include <type_traits>

namespace M1sakalib {

    template<typename T>
    class function;

    template<typename R, typename ...Args>
    class function<R(Args...)> {

        struct CallableBase {
            virtual R invoke(Args...args) = 0;

            virtual std::unique_ptr<CallableBase> clone() = 0;

            virtual const std::type_info &target_type() const noexcept = 0;

            virtual void *target() noexcept = 0;

            virtual ~CallableBase() = default;
        };

        template<typename F>
        struct Callable : CallableBase {

            template<typename U>
            Callable(U &&other) : callable_func_(std::forward<U>(other)) {}

            R invoke(Args...args) override {
                return callable_func_(std::forward<Args>(args)...);
            }

            std::unique_ptr<CallableBase> clone() override {
                return std::make_unique<Callable>(callable_func_);
            }

            const std::type_info &target_type() const noexcept override {
                return typeid(F);
            }

            void *target() noexcept override {
                return &callable_func_;
            }

            F callable_func_;
        };


        std::unique_ptr<CallableBase> callable_;

    public:
        template<typename F>
        function(F &&f):callable_(std::make_unique<Callable<std::decay_t<F>>>(std::forward<F>(f))) {}

        function(function &&other) noexcept: callable_(std::move(other.callable_)) {}

        function(const function &other) : callable_(other.callable_ ? other.callable_->clone() : nullptr) {}

        function() noexcept: callable_(nullptr) {}

        function &operator=(function &&other) noexcept {
            callable_ = std::move(other.callable_);
        }

        function &operator=(const function &other) {
            if (this != &other) {
                callable_ = other.callable_ ? other.callable_->clone() : nullptr;
            }
            return *this;
        }

        R operator()(Args...args) {
            return callable_->invoke(std::forward<Args>(args)...);
        }

        explicit operator bool() const noexcept {
            return static_cast<bool>(callable_);
        }

        const std::type_info &target_type() noexcept {
            return callable_ ? callable_->target_type() : typeid(void);
        }

        template<typename T>
        T *target() noexcept {
            if (callable_ && target_type() == typeid(T)) {
                return static_cast<T *>(callable_->target());
            }
            return nullptr;
        }

        template<typename T>
        const T *target() const noexcept {
            if (callable_ && target_type() == typeid(T)) {
                return static_cast<const T *>(callable_->target());
            }
            return nullptr;
        }
    };

    template<typename R, typename ...Args>
    function(R(Args...)) -> function<R(Args...)>;
}

#endif //M1SAKALIB_FUNCTION_H
