#pragma once

#include <coroutine>
#include <exception>
#include <optional>
#include <stdexcept>
#include <utility>

struct Receive {};

template <typename Out, typename In>
class DuplexCoroutine {
private:
    struct InputAwaiter {
        const In& input;
        bool ready;

        bool await_ready() const noexcept { return ready; }
        void await_suspend(std::coroutine_handle<>) const noexcept {}
        In await_resume() const { return input; }
    };

public:
    struct promise_type {
        std::optional<Out> output;
        In input{};
        std::exception_ptr exception;

        DuplexCoroutine get_return_object() { return DuplexCoroutine(std::coroutine_handle<promise_type>::from_promise(*this)); }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() noexcept { exception = std::current_exception(); }

        InputAwaiter yield_value(Out value) noexcept {
            output = value;
            return InputAwaiter{input, false};
        }

        InputAwaiter await_transform(Receive) noexcept { return InputAwaiter{input, true}; }
    };

    DuplexCoroutine(const DuplexCoroutine&) = delete;
    DuplexCoroutine& operator=(const DuplexCoroutine&) = delete;
    DuplexCoroutine& operator=(DuplexCoroutine&&) = delete;
    DuplexCoroutine(DuplexCoroutine&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}

    ~DuplexCoroutine() {
        if (handle) {
            handle.destroy();
        }
    }

    std::optional<Out> send(In value) {
        if (!handle || handle.done()) {
            throw std::logic_error("спроба відновити завершену сопрограму");
        }
        promise_type& promise = handle.promise();
        promise.input = value;
        promise.output.reset();
        handle.resume();
        if (promise.exception) {
            std::rethrow_exception(std::exchange(promise.exception, nullptr));
        }
        return promise.output;
    }

    std::optional<Out> next() { return send(In{}); }

private:
    explicit DuplexCoroutine(std::coroutine_handle<promise_type> h) : handle(h) {}

    std::coroutine_handle<promise_type> handle;
};