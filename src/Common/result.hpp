#pragma once

#include <pch.hpp>

// Base case: Result<T, E> for normal types
template <typename T, typename E>
class Result {
public:
  struct SuccessTag {};
  struct ErrorTag {};

  static Result success(const T& value) {
    return Result(SuccessTag{}, value);
  }

  static Result error(const E& error) {
    return Result(ErrorTag{}, error);
  }

  bool is_success() const {
    return success_value.has_value();
  }

  bool is_error() const {
    return error_value.has_value();
  }

  std::optional<T> get_success() const {
    return success_value;
  }

  std::optional<E> get_error() const {
    return error_value;
  }

private:
  Result(SuccessTag, const T& value) : success_value(value), error_value(std::nullopt) {
  }
  Result(ErrorTag, const E& error) : success_value(std::nullopt), error_value(error) {
  }

  std::optional<T> success_value;
  std::optional<E> error_value;
};

// Specialization for T = void
template <typename E>
class Result<void, E> {
public:
  struct SuccessTag {};
  struct ErrorTag {};

  static Result success() {
    return Result(SuccessTag{});
  }

  static Result error(const E& error) {
    return Result(ErrorTag{}, error);
  }

  bool is_success() const {
    return is_success_;
  }

  bool is_error() const {
    return !is_success_;
  }

  std::optional<E> get_error() const {
    return is_error() ? std::optional<E>(error_value) : std::nullopt;
  }

private:
  Result(SuccessTag) : is_success_(true) {
  }
  Result(ErrorTag, const E& error) : is_success_(false), error_value(error) {
  }

  bool is_success_ = false;
  E error_value;
};
