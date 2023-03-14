#pragma once

#include "array_ptr.h"

#include <algorithm>
#include <initializer_list>
#include <stdexcept>

class ReserveProxyObj {
public:
	ReserveProxyObj() noexcept = default;
	
	ReserveProxyObj(size_t capacity_to_reserve) 
	: capacity_to_reserve_(capacity_to_reserve) {
	}
	
	size_t GetCapacity() const noexcept {
		return capacity_to_reserve_;
	}
private:
	size_t capacity_to_reserve_ = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
	return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
	
    using Iterator = Type*;
    using ConstIterator = const Type*;
	
    SimpleVector() noexcept = default;
	
	SimpleVector(ReserveProxyObj capacity) {
		Reserve(capacity.GetCapacity());
		size_ = 0;
	}
	
    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
	explicit SimpleVector(size_t size)
		: arr_(size)
		, size_(size)
		, capacity_(size) {
		std::generate(begin(), end(), [] () { return Type(); });
    }
	
    // Создаёт вектор из size элементов, инициализированных значением value
	SimpleVector(size_t size, const Type& value)
		: SimpleVector(size) {
		std::fill(begin(), end(), value);
    }
	
    // Создаёт вектор из std::initializer_list
	SimpleVector(std::initializer_list<Type> init)
		: arr_(init.size())
		, size_(init.size())
		, capacity_(init.size()) {
		std::move(init.begin(), init.end(), begin());
    }
	
	//Конструктор копирования
	SimpleVector(const SimpleVector& other) {
		if (&other != this) {
			SimpleVector new_arr(other.size_);
			std::copy(other.begin(), other.end(), new_arr.begin());
			swap(new_arr);
		}
	}
	//move-constructor для некопируемых
	SimpleVector(SimpleVector&& rhs)
		: arr_(std::exchange(rhs.arr_, {}))
		, size_(std::exchange(rhs.size_, 0))
		, capacity_(std::exchange(rhs.capacity_, 0)) {
	}
	
	//Оператор присваивания
	SimpleVector& operator=(const SimpleVector& rhs) {
		if (this != &rhs) {
			SimpleVector tmp(rhs);
			swap(tmp);
		}
		return *this;
	}

	// move- operator=
	SimpleVector& operator=(SimpleVector&& rhs) {
		if (this != &rhs) {
			SimpleVector tmp(std::move(rhs));
			swap(tmp);
		}
		return *this;
	}
	
	void Reserve(size_t new_capacity) {
		if (capacity_ < new_capacity) {
			ArrayPtr<Type> new_arr(new_capacity);
			std::move(begin(), end(), new_arr.Get());
			arr_.swap(new_arr);
			capacity_ = new_capacity;
		}
	}

	// Добавляет элемент в конец вектора
	// При нехватке места увеличивает вдвое вместимость вектора
	void PushBack(const Type& item) {
		Iterator it = MakeIterToPushBack();
		*it = item;
	}
	
	void PushBack(Type&& item) {
		Iterator it = MakeIterToPushBack();
		*it = std::move(item);
	}
	
	// Вставляет значение value в позицию pos.
	// Возвращает итератор на вставленное значение
	// Если перед вставкой значения вектор был заполнен полностью,
	// вместимость вектора увеличивается вдвое, а для вектора вместимостью 0 становится равной 1
	Iterator Insert(ConstIterator pos, const Type& value) {
		Iterator it = PreparedIterToInsert(pos);
		*it = value;
		return it;
	}
	
	Iterator Insert(ConstIterator pos, Type&& value) {
		Iterator it = MakeIterToInsert(pos);
		*it = std::move(value);
		return it;
	}
	
	// "Удаляет" последний элемент вектора. Вектор не должен быть пустым
	void PopBack() noexcept {
		assert(!IsEmpty());
		--size_;
	}
	
	// Удаляет элемент вектора в указанной позиции. copy-метод для конст
	Iterator Erase(ConstIterator pos) {
		return MakeIterForErase(pos);
	}
	
	// Удаляет элемент вектора в указанной позиции. move-метод для неконст
	Iterator Erase(Iterator pos) {
		return MakeIterForErase(pos);
	}
		
	// Обменивает значение с другим вектором
	void swap(SimpleVector& other) noexcept {
		arr_.swap(other.arr_);
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
	}
	
    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
	Type& operator[](size_t index) noexcept {
		assert(index <= size_); 
		return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
		assert(index <= size_);
		return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
	Type& At(size_t index) {
		if (index >= size_) {
			throw std::out_of_range("Index is out of range");
		}
		return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
		if (index >= size_) {
			throw std::out_of_range("Index is out of range");
		}
		return arr_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
		size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
	void Resize(size_t new_size)
	{
		if (new_size <= size_) {
			size_ = new_size;
			return;
		} else if (new_size > capacity_) {
			Reserve(new_size);
			std::generate(begin() + size_, end(), []() {return Type{}; });
		} else {
			std::generate(begin() + size_, end() + new_size, []() {return Type{}; });
		}
		size_ = new_size;
	}

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
		return arr_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
		return arr_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
		return arr_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
		return arr_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
		return arr_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
		return arr_.Get() + size_;
    }
private:
	ArrayPtr<Type> arr_ {};
	size_t size_ = 0;
	size_t capacity_ = 0;
	
	size_t GetNewCapacity (const size_t& new_size) const {
		size_t new_capacity = capacity_ != 0
				? new_size > capacity_ ? new_size * 2 : capacity_ : 1;
		return new_capacity;
	}
	
	bool IsNotAvailableCapacity (const size_t& new_size) const {
		return new_size > capacity_;
	}
	
	Iterator MakeIterToPushBack() {
		size_t new_size = size_ + 1;
		size_t new_capacity = GetNewCapacity(new_size);
		if (IsNotAvailableCapacity(new_size)) {
			Reserve(new_capacity);
		}
		size_ = new_size;
		capacity_ = new_capacity;
		size_t current_pos = size_ - 1;
		return Iterator { arr_.Get() + current_pos };
	}
	
	template<typename Iter>
	Iterator MakeIterToInsert(Iter pos) {
		Iterator it = Iterator(pos);
		auto current_pos = it - begin();
		size_t new_capacity = GetNewCapacity(size_ + 1);
		if (IsNotAvailableCapacity(size_ + 1)) {
			Reserve(new_capacity);
		}
		++size_;
		it = begin() + current_pos;
		std::move_backward(it, end(), end() + 1);
		return it;
	}
	
	template <typename Iter>
	Iterator MakeIterForErase(Iter pos) {
		Iterator it = Iterator(pos);
		auto current_pos = begin() + std::distance(begin(), it);
		if (size_ > 0) {
			std::move(it + 1, end(), current_pos);
			--size_;
		}
		return begin() + std::distance(begin(), it);
	}
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return (lhs.GetSize() == rhs.GetSize())
			&& std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return std::lexicographical_compare(
				lhs.begin(), lhs.end(),
				rhs.begin(), rhs.end(),
				[](const auto& lhs, const auto& rhs) {
					return lhs < rhs;
				});
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return !(lhs < rhs);
} 
