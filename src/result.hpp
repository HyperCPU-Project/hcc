#pragma once

#include <pch.hpp>

// Forward declaration of void specialization
template <typename E> class Result<void, E>;

template <typename T, typename E> class Result {
public:
	// Tag types for constructor disambiguation
	struct SuccessTag {};
	struct ErrorTag {};

	// Create successful result with value
	static Result success(const T& value) {
		return Result(SuccessTag{}, value);
	}

	// Create error result with error
	static Result error(const E& error) {
		return Result(ErrorTag{}, error);
	}

	// Check if result is successful
	bool is_success() const {
		return success_value.has_value();
	}

	// Check if result is error
	bool is_error() const {
		return error_value.has_value();
	}

	// Get success value (returns std::nullopt if error)
	std::optional<T> get_success() const {
		return success_value;
	}

	// Get error value (returns std::nullopt if success)
	std::optional<E> get_error() const {
		return error_value;
	}

private:
	// Private tagged constructors
	Result(SuccessTag, const T& value) : success_value(value) {
	}
	Result(ErrorTag, const E& error) : error_value(error) {
	}

	std::optional<T> success_value;
	std::optional<E> error_value;
};

// Specialization for void success type
template <typename E> class Result<void, E> {
public:
	struct SuccessTag {};
	struct ErrorTag {};

	// Create successful result (no value needed for void)
	static Result<void, E> success() {
		return Result(SuccessTag{});
	}

	// Create error result with error
	static Result<void, E> error(const E& error) {
		return Result(ErrorTag{}, error);
	}

	// Check if result is successful
	bool is_success() const {
		return is_success_;
	}

	// Check if result is error
	bool is_error() const {
		return error_value.has_value();
	}

	// Get error value (returns std::nullopt if success)
	std::optional<E> get_error() const {
		return error_value;
	}

private:
	// Private tagged constructors
	Result(SuccessTag) : is_success_(true) {
	}
	Result(ErrorTag, const E& error) : error_value(error), is_success_(false) {
	}

	std::optional<E> error_value;
	bool is_success_;
};
