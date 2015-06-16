#pragma once
#ifndef ECUDA_MODELS_HPP
#define ECUDA_MODELS_HPP

#include "memory.hpp"
#include "iterator.hpp"

namespace ecuda {

///
/// \brief Base representation of a device-bound sequence.
///
/// This class makes no assumptions about the contiguity of the allocated memory.
/// The pointer specialization is fully responsible for traversing the sequence.
///
template<typename T,class PointerType>
class __device_sequence
{

public:
	typedef T value_type;
	typedef PointerType pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	typedef device_iterator<value_type,typename pointer_traits<pointer>::modifiable_pointer> iterator;
	typedef device_iterator<const value_type,typename pointer_traits<typename pointer_traits<pointer>::const_pointer>::modifiable_pointer> const_iterator;
	typedef reverse_device_iterator<iterator> reverse_iterator;
	typedef reverse_device_iterator<const_iterator> const_reverse_iterator;

private:
	pointer ptr;
	size_type length;

protected:
	__host__ __device__ inline pointer& get_pointer() { return ptr; }
	__host__ __device__ inline const pointer& get_pointer() const { return ptr; }

public:
	__host__ __device__ __device_sequence( pointer ptr = pointer(), size_type length = 0 ) : ptr(ptr), length(length) {}
	__host__ __device__ __device_sequence( const __device_sequence& src ) : ptr(src.ptr), length(src.length) {}

	__host__ __device__ inline size_type size() const { return length; }

// \todo This seems wrong - seems to assume contiguity.
	__device__ inline reference operator[]( const size_type x ) { return *(pointer_traits<pointer>().undress(ptr)+x); }
	__device__ inline const_reference operator[]( const size_type x ) const { return *(pointer_traits<typename pointer_traits<pointer>::const_pointer>().undress(ptr)+x); }

	__host__ __device__ inline iterator begin() { return iterator( pointer_traits<pointer>().cast_to_modifiable(ptr) ); }
	__host__ __device__ inline iterator end() { return iterator( pointer_traits<pointer>().increment(ptr,size()) ); }
	__host__ __device__ inline const_iterator begin() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().cast_to_modifiable(ptr) ); }
	__host__ __device__ inline const_iterator end() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().increment(ptr,size()) ); }
	#ifdef __CPP11_SUPPORTED__
	__host__ __device__ inline const_iterator cbegin() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().cast_to_modifiable(ptr) ); }
	__host__ __device__ inline const_iterator cend() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().increment(ptr,size()) ); }
	#endif

	__host__ __device__ inline reverse_iterator rbegin() { return reverse_iterator(end()); }
	__host__ __device__ inline reverse_iterator rend() { return reverse_iterator(begin()); }
	__host__ __device__ inline const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	__host__ __device__ inline const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
	#ifdef __CPP11_SUPPORTED__
	__host__ __device__ inline const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
	__host__ __device__ inline const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }
	#endif

	__host__ __device__ void swap( __device_sequence& other ) {
		#ifdef __CUDA_ARCH__
		iterator iter1 = begin();
		iterator iter2 = other.begin();
		for( ; iter1 != end(); ++iter1, ++iter2 ) ecuda::swap( *iter1, *iter2 );
		#else
		std::swap( ptr, other.ptr );
		std::swap( length, other.length );
		#endif
	}

};

///
/// \brief Base representation of a fixed-size device-bound sequence.
///
/// This class assumes the allocated memory is contiguous in order to function
/// properly.
///
template<typename T,std::size_t N,class PointerType=typename type_traits<T>::pointer>
class __device_fixed_sequence
{

public:
	typedef T value_type;
	typedef PointerType pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	typedef device_contiguous_iterator<value_type> iterator;
	typedef device_contiguous_iterator<const value_type> const_iterator;
	typedef reverse_device_iterator<iterator> reverse_iterator;
	typedef reverse_device_iterator<const_iterator> const_reverse_iterator;

private:
	pointer ptr;

protected:
	__host__ __device__ inline pointer& get_pointer() { return ptr; }
	__host__ __device__ inline const pointer& get_pointer() const { return ptr; }

public:
	__host__ __device__ __device_fixed_sequence( pointer ptr = pointer() ) : ptr(ptr) {}
	__host__ __device__ __device_fixed_sequence( const __device_fixed_sequence& src ) : ptr(src.ptr) {}

	__host__ __device__ inline __CONSTEXPR__ size_type size() const { return N; }

	__device__ inline reference operator[]( const size_type x ) { return *(pointer_traits<pointer>().undress(ptr) + x); }
	__device__ inline const_reference operator[]( const size_type x ) const { return *(pointer_traits<typename pointer_traits<pointer>::const_pointer>().undress(ptr) + x); }

	__host__ __device__ inline iterator begin() { return iterator( pointer_traits<pointer>().undress(ptr) ); }
	__host__ __device__ inline iterator end() { return iterator( pointer_traits<pointer>().undress(ptr)+N ); }
	__host__ __device__ inline const_iterator begin() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().undress(ptr) ); }
	__host__ __device__ inline const_iterator end() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().undress(ptr)+N ); }
	#ifdef __CPP11_SUPPORTED__
	__host__ __device__ inline const_iterator cbegin() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().undress(ptr) ); }
	__host__ __device__ inline const_iterator cend() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().undress(ptr)+N ); }
	#endif

	__host__ __device__ inline reverse_iterator rbegin() { return reverse_iterator(end()); }
	__host__ __device__ inline reverse_iterator rend() { return reverse_iterator(begin()); }
	__host__ __device__ inline const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	__host__ __device__ inline const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
	#ifdef __CPP11_SUPPORTED__
	__host__ __device__ inline const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
	__host__ __device__ inline const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }
	#endif

	__host__ __device__ void swap( __device_fixed_sequence& other ) {
		#ifdef __CUDA_ARCH__
		iterator iter1 = begin();
		iterator iter2 = other.begin();
		for( ; iter1 != end(); ++iter1, ++iter2 ) ecuda::swap( *iter1, *iter2 );
		#else
		std::swap( ptr, other.ptr );
		#endif
	}

};

///
/// \brief Base representation of a contiguous device-bound sequence.
///
/// This class assumes the allocated memory is contiguous in order to function
/// properly.
///
template<typename T,class PointerType=typename type_traits<T>::pointer>
class __device_contiguous_sequence : public __device_sequence<T,PointerType>
{
private:
	typedef __device_sequence<T,PointerType> base_type;

public:
	typedef typename base_type::value_type value_type;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::reference reference;
	typedef typename base_type::const_reference const_reference;
	typedef typename base_type::size_type size_type;
	typedef typename base_type::difference_type difference_type;

	typedef device_contiguous_iterator<value_type> iterator;
	typedef device_contiguous_iterator<const value_type> const_iterator;
	typedef reverse_device_iterator<iterator> reverse_iterator;
	typedef reverse_device_iterator<const_iterator> const_reverse_iterator;

public:
	__host__ __device__ __device_contiguous_sequence( pointer ptr = pointer(), size_type length = 0 ) : base_type(ptr,length) {}
	__host__ __device__ __device_contiguous_sequence( const __device_contiguous_sequence& src ) : base_type(src) {}

	__host__ __device__ iterator begin() { return iterator( pointer_traits<pointer>().undress(base_type::get_pointer()) ); }
	__host__ __device__ iterator end() { return iterator( pointer_traits<pointer>().undress(base_type::get_pointer()) + base_type::size() ); }
	__host__ __device__ const_iterator begin() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().undress(base_type::get_pointer()) ); }
	__host__ __device__ const_iterator end() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().undress(base_type::get_pointer()) + base_type::size() ); }
	#ifdef __CPP11_SUPPORTED__
	__host__ __device__ const_iterator cbegin() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().undress(base_type::get_pointer()) ); }
	__host__ __device__ const_iterator cend() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().undress(base_type::get_pointer()) + base_type::size() ); }
	#endif

	__host__ __device__ reverse_iterator rbegin() { return reverse_iterator(end()); }
	__host__ __device__ reverse_iterator rend() { return reverse_iterator(begin()); }
	__host__ __device__ const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	__host__ __device__ const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
	#ifdef __CPP11_SUPPORTED__
	__host__ __device__ const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
	__host__ __device__ const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }
	#endif

};

///
/// \brief Base representation of a device-bound matrix.
///
/// This class makes no assumptions about the contiguity of the allocated memory.
/// The pointer specialization is fully responsible for traversing the matrix.
///
template<typename T,class PointerType>
class __device_matrix : public __device_sequence<T,PointerType>
{
private:
	typedef __device_sequence<T,PointerType> base_type;

public:
	typedef typename base_type::value_type value_type;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::reference reference;
	typedef typename base_type::const_reference const_reference;
	typedef typename base_type::size_type size_type;
	typedef typename base_type::difference_type difference_type;

	typedef typename base_type::iterator iterator;
	typedef typename base_type::const_iterator const_iterator;
	typedef typename base_type::reverse_iterator reverse_iterator;
	typedef typename base_type::const_reverse_iterator const_reverse_iterator;

	typedef __device_sequence<value_type,typename pointer_traits<pointer>::modifiable_pointer> row_type;
	typedef __device_sequence<const value_type,typename pointer_traits<typename pointer_traits<pointer>::modifiable_pointer>::const_pointer> const_row_type;
	typedef __device_sequence< value_type, striding_ptr<value_type,typename pointer_traits<pointer>::modifiable_pointer> > column_type;
	typedef __device_sequence< const value_type, striding_ptr<const value_type,typename pointer_traits<typename pointer_traits<pointer>::const_pointer>::modifiable_pointer> > const_column_type;

private:
	size_type rows;

public:
	__host__ __device__ __device_matrix( pointer ptr = pointer(), size_type rows = 0, size_type columns = 0 ) : base_type(ptr,rows*columns), rows(rows) {}
	__host__ __device__ __device_matrix( const __device_matrix& src ) : base_type(src), rows(src.rows) {}

	__host__ __device__ inline size_type number_rows() const { return rows; }
	__host__ __device__ inline size_type number_columns() const { return base_type::size()/rows; }

	__host__ __device__ inline row_type get_row( const size_type row ) { return row_type( pointer_traits<pointer>().increment(base_type::get_pointer(),row*number_columns()), number_columns() ); }
	__host__ __device__ inline const_row_type get_row( const size_type row ) const { return const_row_type( pointer_traits<typename pointer_traits<pointer>::const_pointer>().increment(base_type::get_pointer(),row*number_columns()), number_columns() ); }

	__host__ __device__ inline column_type get_column( const size_type column ) {
		return column_type( 
			striding_ptr<value_type,typename pointer_traits<pointer>::modifiable_pointer>(
				pointer_traits<pointer>().increment( base_type::get_pointer(), column ),
				number_columns() 
			),
			number_rows()
		); 
	}
	__host__ __device__ inline const_column_type get_column( const size_type column ) const { 
		return const_column_type( 
			striding_ptr<const value_type,typename pointer_traits<typename pointer_traits<pointer>::const_pointer>::modifiable_pointer>(
				pointer_traits<typename pointer_traits<pointer>::const_pointer>().increment( base_type::get_pointer(), column ),
				//pointer_traits<typename pointer_traits<pointer>::const_pointer>().make_offsetable( base_type::get_pointer() ) + column,
				number_columns() 
			),
			number_rows()
		); 
	}

	__host__ __device__ inline row_type operator[]( const size_type row ) { return get_row(row); }
	__host__ __device__ inline const_row_type operator[]( const size_type row ) const { return get_row(row); }

	//__host__ __device__ inline value_type at( const size_type rowIndex, const size_type columnIndex ) { get_pointer()

};

///
/// \brief Base representation of a device-bound matrix where each row is contiguous.
///
/// This class assumes the allocated memory represents each row in contiguous memory
/// in order to function properly. It is not assumed that the row formation of the
/// matrix is contiguous (e.g. padding between each row).
///
template<typename T,class PointerType>
class __device_contiguous_row_matrix : public __device_matrix<T,PointerType>
{
private:
	typedef __device_matrix<T,PointerType> base_type;

public:
	typedef typename base_type::value_type value_type;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::reference reference;
	typedef typename base_type::const_reference const_reference;
	typedef typename base_type::size_type size_type;
	typedef typename base_type::difference_type difference_type;

	typedef typename base_type::iterator iterator;
	typedef typename base_type::const_iterator const_iterator;
	typedef typename base_type::reverse_iterator reverse_iterator;
	typedef typename base_type::const_reverse_iterator const_reverse_iterator;

	typedef __device_contiguous_sequence<value_type> row_type;
	typedef __device_contiguous_sequence<const value_type> const_row_type;
	typedef typename base_type::column_type column_type;
	typedef typename base_type::const_column_type const_column_type;

public:
	__host__ __device__ __device_contiguous_row_matrix( pointer ptr = pointer(), size_type rows = 0, size_type columns = 0 ) : base_type(ptr,rows,columns) {}
	__host__ __device__ __device_contiguous_row_matrix( const __device_contiguous_row_matrix& src ) : base_type(src) {}

	__host__ __device__ inline iterator begin() { return iterator( pointer_traits<pointer>().cast_to_modifiable(base_type::get_pointer()) ); }
	__host__ __device__ inline iterator end() { return iterator( pointer_traits<pointer>().increment(base_type::get_pointer(),base_type::size()) ); }
	__host__ __device__ inline const_iterator begin() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().cast_to_modifiable(base_type::get_pointer()) ); }
	__host__ __device__ inline const_iterator end() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().increment(base_type::get_pointer(),base_type::size()) ); }
	#ifdef __CPP11_SUPPORTED__
	__host__ __device__ inline const_iterator cbegin() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().cast_to_modifiable(base_type::get_pointer()) ); }
	__host__ __device__ inline const_iterator cend() const { return const_iterator( pointer_traits<typename pointer_traits<pointer>::const_pointer>().increment(base_type::get_pointer(),base_type::size()) ); }
	#endif

	__host__ __device__ reverse_iterator rbegin() { return reverse_iterator(end()); }
	__host__ __device__ reverse_iterator rend() { return reverse_iterator(begin()); }
	__host__ __device__ const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	__host__ __device__ const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
	#ifdef __CPP11_SUPPORTED__
	__host__ __device__ const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
	__host__ __device__ const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }
	#endif


	__host__ __device__ inline row_type get_row( const size_type row ) {
		typename pointer_traits<pointer>::modifiable_pointer mp = pointer_traits<pointer>().increment( base_type::get_pointer(), row*base_type::number_columns() );
		return row_type( pointer_traits<typename pointer_traits<pointer>::modifiable_pointer>().undress(mp), base_type::number_columns() );
	}
	__host__ __device__ inline const_row_type get_row( const size_type row ) const {
		typename pointer_traits<typename pointer_traits<pointer>::const_pointer>::modifiable_pointer mp = pointer_traits<typename pointer_traits<pointer>::const_pointer>().increment( base_type::get_pointer(), row*base_type::number_columns() );
		return const_row_type( pointer_traits<typename pointer_traits<typename pointer_traits<pointer>::const_pointer>::modifiable_pointer>().undress(mp), base_type::number_columns() );
	}

	__host__ __device__ inline row_type operator[]( const size_type row ) { return get_row(row); }
	__host__ __device__ inline const_row_type operator[]( const size_type row ) const { return get_row(row); }

	__host__ __device__ inline reference at( const size_type row, const size_type column ) {
		typename pointer_traits<pointer>::modifiable_pointer mp = pointer_traits<pointer>().increment( base_type::get_pointer(), row*base_type::number_columns()+column );
		return *pointer_traits<typename pointer_traits<pointer>::modifiable_pointer>::undress( mp );
	}

	__host__ __device__ inline const_reference at( const size_type row, const size_type column ) const {
		typename pointer_traits<typename pointer_traits<pointer>::const_pointer>::modifiable_pointer mp = pointer_traits<typename pointer_traits<pointer>::const_pointer>().increment( base_type::get_pointer(), row*base_type::number_columns()+column );
		return *pointer_traits<typename pointer_traits<typename pointer_traits<pointer>::const_pointer>::modifiable_pointer>::undress( mp );
	}

};



} // namespace ecuda

#endif
