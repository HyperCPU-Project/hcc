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

  bool isSuccess() const {
    return successValue.has_value();
  }

  bool isError() const {
    return errorValue.has_value();
  }

  std::optional<T> getSuccess() const {
    return successValue;
  }

  std::optional<E> getError() const {
    return errorValue;
  }

private:
  Result(SuccessTag, const T& value) : successValue(value), errorValue(std::nullopt) {
  }
  Result(ErrorTag, const E& error) : successValue(std::nullopt), errorValue(error) {
  }

  std::optional<T> successValue;
  std::optional<E> errorValue;
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
    return isSuccess_;
  }

  bool is_error() const {
    return !isSuccess_;
  }

  std::optional<E> get_error() const {
    return is_error() ? std::optional<E>(errorValue) : std::nullopt;
  }

private:
  Result(SuccessTag) : isSuccess_(true) {
  }
  Result(ErrorTag, const E& error) : isSuccess_(false), errorValue(error) {
  }

  bool isSuccess_ = false;
  E errorValue;
};
