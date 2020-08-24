#include <stdint.h>
#include "ErrorCode.h"
#include "Matrix.h"

class IMatrix {
private:
	PMatrix data;
public:
	/**
	 * @brief Constructor. Uses given height and width,
	 *  all values are initially zeroes.
	 *
	 * @param[in] height Height of the IMatrix
	 * @param[in] width Width of the IMatrix
	 * @throws ErrorCode If an error occured
	 */
	IMatrix(int height, int width) {
		ErrorCode ec = matrix_create(&data, height, width);
		if (!error_isSuccess(ec)) {
			throw ec;
		}
	}

	/**
	 * @brief Copy constructor.
	 *
	 * @param[in] copyFrom The copied IMatrix
	 * @throws ErrorCode If an error occured from the copying
	 */
	IMatrix(const IMatrix& copyFrom) {
		*this = copyFrom;
	}

	/**
	 * @brief Move constructor.
	 *
	 * @param[in] mat The RValue IMatrix
	 */
	IMatrix(IMatrix&& mat) {
		*this = mat;
	}

	/**
	 * @brief Destroys an IMatrix.
	 */
	~IMatrix() {
		matrix_destroy(data);
	}

	/**
	 * @brief Copies another IMatrix into this IMatrix.
	 *
	 * @param[in] copyFrom The copied IMatrix
	 * @throws ErrorCode If an error occured
	 * @return Reference to this IMatrix
	 */
	IMatrix& operator=(const IMatrix& copyfrom) {
		ErrorCode ec = matrix_copy(&data, copyfrom.data);
		if (!error_isSuccess(ec)) {
			throw ec;
		}
		return *this;
	}

	/**
	 * @brief Copies an RValue IMatrix object into this IMatrix.
	 *
	 * @param[in] copyFrom The copied RValue IMatrix
	 * @return Reference to this IMatrix
	 */
	IMatrix& operator=(IMatrix&& copyFrom) {
		data = copyFrom.data;
		copyFrom.data = nullptr;
		return *this;
	}

	/**
	 * @brief Returns the height.
	 *
	 * @throws ErrorCode If an error occured
	 * @return The height of the IMatrix
	 */
	uint32_t getHeight() const {
		uint32_t height;
		ErrorCode ec = matrix_getHeight(data, &height);
		if (!error_isSuccess(ec)) {
			throw ec;
		}
		return height;
	}

	/**
	 * @brief Returns the width.
	 *
	 * @throws ErrorCode If an error occured
	 * @return The width of the IMatrix
	 */
	uint32_t getWidth() const {
		uint32_t width;
		ErrorCode ec = matrix_getWidth(data, &width);
		if (!error_isSuccess(ec)) {
			throw ec;
		}
		return width;
	}

	/**
	 * @brief Sets a value to the IMatrix.
	 *
	 * @param[in] rowIndex Row index of the value to set.
	 * @param[in] colIndex Column index of the value to setF.
	 * @param[in] value Value to set.
	 * @throws ErrorCode If an error occured
	 */
	void setValue(uint32_t rowIndex, uint32_t colIndex, double value) {
		ErrorCode ec = matrix_setValue(data, rowIndex, colIndex, value);
		if (!error_isSuccess(ec)) {
			throw ec;
		}
	}

	/**
	 * @brief Gets a value from this IMatrix.
	 *
	 * @param[in] rowIndex Row index of the value to get.
	 * @param[in] colIndex Column index of the value to get.
	 * @throws ErrorCode If an error occured
	 * @return The value at the right row and col
	 */
	double getValue(uint32_t rowIndex, uint32_t colIndex) const {
		double value;
		ErrorCode ec = matrix_getValue(data, rowIndex, colIndex, &value);
		if (!error_isSuccess(ec)) {
			throw ec;
		}
		return value;
	}

	/**
	 * @brief Computes the addition of this IMatrix and another IMatrix.
	 *
	 * @param[in] rhs The right hand side of the addition operation.
	 * @throws ErrorCode If an error occured
	 * @return The result IMatrix
	 */
	IMatrix operator+(const IMatrix& rhs) const {
		IMatrix result = *this;
		result += rhs;
		return result;
	}

	/**
	 * @brief Adds another IMatrix to this IMatrix and copies the result to this IMatrix.
	 *
	 * @param[in] mat The IMatrix which is added to this IMatrix.
	 * @throws ErrorCode If an error occured from the addition
	 * @return Reference to this IMatrix
	 */
	IMatrix& operator+=(const IMatrix& mat) {
		ErrorCode ec = matrix_add(&data, data, mat.data);
		if (!error_isSuccess(ec)) {
			throw ec;
		}
		return *this;
	}

	/**
	 * @brief Computes the diffrence between this IMatrix and another IMatrix.
	 *
	 * @param[in] rhs The right hand side of the diffrence operation.
	 * @throws ErrorCode If an error occured
	 * @return The result IMatrix
	 */
	IMatrix operator-(const IMatrix& rhs) const {
		IMatrix result = *this;
		result -= rhs;
		return result;
	}

	/**
	 * @brief Subtracts another IMatrix from this IMatrix and copies the result to this IMatrix.
	 *
	 * @param[in] mat The IMatrix which is subtracted from this IMatrix.
	 * @throws ErrorCode If an error occured from the subtraction
	 * @return Reference to this IMatrix
	 */
	IMatrix& operator-=(const IMatrix& mat) {
		ErrorCode ec = matrix_add(&data, data, (-mat).data);
		if (!error_isSuccess(ec)) {
			throw ec;
		}
		return *this;
	}

	/**
	 * @brief Computes the multiplication of this IMatrix with scalar 1
	 *
	 * @return The result
	 */
	IMatrix operator-() {
		return *this * -1;
	}

	/**
	 * @brief Computes the multiplication of this IMatrix and another IMatrix.
	 *
	 * @param[in] rhs The right hand side of the multiplication operation.
	 * @throws ErrorCode If an error occured
	 * @return The result IMatrix
	 */
	IMatrix operator*(const IMatrix& rhs) const {
		IMatrix result;
		ErrorCode ec = matrix_multiplyMatrices(&(result.data), data, rhs.data);
		if (!error_isSuccess(ec)) {
			throw ec;
		}
		return result;
	}

	/**
	 * @brief Computes the multiplication of this IMatrix with a scalar.
	 *
	 * @param[in] scalar The scalar to multiply with.
	 * @throws ErrorCode If an error occured
	 * @return The result IMatrix
	 */
	IMatrix operator*(double scalar) const {
		IMatrix result = *this;
		result *= scalar;
		return result;
	}

	friend IMatrix operator*(double scalar, const IMatrix& mat);

	/**
	 * @brief Multiplies this IMatrix with a scalar and copies the result to this IMatrix.
	 *
	 * @param[in] scalar The scalar that multiplies this IMatrix
	 * @throws ErrorCode If an error occured from the multiplication
	 * @return Reference to this IMatrix
	 */
	IMatrix& operator*=(double scalar) {
		ErrorCode ec = matrix_multiplyWithScalar(data, scalar);
		if (!error_isSuccess(ec)) {
			throw ec;
		}
		return *this;
	}
};

/**
 * @brief Extends operator*(double scalar) of IMatrix so it also works when the scalar
 *  is to the left of the IMatrix.
 *
 * @param[in] scalar The scalar to multiply with.
 * @param[in] mat The IMatrix to multiply with.
 * @throws ErrorCode If an error occured from the multiplication
 * @return The result IMatrix
 */
IMatrix operator*(double scalar, const IMatrix& mat) {
	return mat * scalar;
}
