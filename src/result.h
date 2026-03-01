#pragma once

struct OkTag {};
struct ErrTag {};

inline constexpr OkTag Ok{};
inline constexpr ErrTag Err{};

template<typename T, typename E>
class Result {
public:
    static Result<T,E> ok(T value) noexcept { return Result{Ok, std::move(value)}; }
    static Result<T,E> err(E error) noexcept { return Result{Err, std::move(error)}; }

    bool is_ok() const noexcept { return is_ok_; }
    const T& value() const noexcept { return value_; }
    const E& error() const noexcept { return error_; }

private:
    Result(OkTag, T value) : is_ok_{true}, value_{std::move(value)} {}
    Result(ErrTag, E error) : is_ok_{false}, error_{std::move(error)} {}

private:
    bool is_ok_;
    union {
        T value_;
        E error_;
    };
};

template<typename E>
class Result<void, E> {
public:
    static Result<void,E> ok() noexcept { return Result{Ok}; }
    static Result<void,E> err(E error) noexcept { return Result{Err, std::move(error)}; }

    bool is_ok() const noexcept { return is_ok_; }
    const E& value() const noexcept { return error_; }

private:
    Result(OkTag) : is_ok_{true} {}
    Result(ErrTag, E error) : is_ok_{false}, error_{std::move(error)} {}
    
private:
    bool is_ok_;
    E error_;
};

template<typename T>
class Result<T, void> {
public:
    static Result<T,void> ok(T value) noexcept { return Result{Ok, std::move(value)}; }
    static Result<T,void> err() noexcept { return Result{Err}; }

    bool is_ok() const noexcept { return is_ok_; }
    const T& value() const noexcept { return value_; }

private:
    Result(OkTag, T value) : is_ok_{true}, value_{std::move(value)} {}
    Result(ErrTag) : is_ok_{false} {}

private:
    bool is_ok_;
    T value_;
};