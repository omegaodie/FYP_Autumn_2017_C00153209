#pragma once
//Mathieu Dutour Sikiric
//Hebrew University of Jerusalem
//https://www.quora.com/How-do-I-represent-a-3D-array-in-column-major-order?share=1 no prizes for guessing what i put in google to find it
template<typename T>
struct ThreeDimArray {
public:
	ThreeDimArray() : n1(0), n2(0), n3(0) {
		ListElt = NULL;
	}
	ThreeDimArray(size_t p1, size_t p2, size_t p3) : n1(p1), n2(p2), n3(p3)
	{
		if (p1 < 0 || p2 < 0 || p3 < 0) {
			std::cerr << "p123=" << p1 << "," << p2 << "," << p3 << "\n";
			exit(1);
		}
		ListElt = new T[n1*n2*n3];
	}
	ThreeDimArray(ThreeDimArray<T> const& eArr)
	{
		std::vector<size_t> LDim = eArr.GetDims();
		n1 = LDim[0];
		n2 = LDim[1];
		n3 = LDim[2];
		size_t len = n1*n2*n3;
		ListElt = new T[n1*n2*n3];
		for (size_t ielt = 0; ielt<len; ielt++) {
			T eVal = eArr.getelt(ielt);
			ListElt[ielt] = eVal;
		}
	}
	ThreeDimArray<T> operator=(ThreeDimArray<T> const& eArr)
	{
		std::vector<size_t> LDim = eArr.GetDims();
		n1 = LDim[0];
		n2 = LDim[1];
		n3 = LDim[2];
		size_t len = n1*n2*n3;
		ListElt = new T[n1*n2*n3];
		for (size_t ielt = 0; ielt<len; ielt++) {
			T eVal = eArr.getelt(ielt);
			ListElt[ielt] = eVal;
		}
		return *this;
	}
	~ThreeDimArray()
	{
		delete[] ListElt;
	}
	std::vector<size_t> GetDims(void) const
	{
		return{ n1,n2,n3 };
	}
	void assign(size_t i1, size_t i2, size_t i3, T const& eVal)
	{
		size_t idx = i1 + n1*i2 + n1*n2*i3;
		ListElt[idx] = eVal;
	}
	T get(size_t i1, size_t i2, size_t i3) const
	{
		size_t idx = i1 + n1*i2 + n1*n2*i3;
		if (i1 > n1 - 1 || i1 < 0) {
			std::cerr << "i1=" << i1 << " n1=" << n1 << "\n";
			exit(1);
		}
		if (i2 > n2 - 1 || i2 < 0) {
			std::cerr << "i2=" << i2 << " n2=" << n2 << "\n";
			exit(1);
		}
		if (i3 > n3 - 1 || i3 < 0) {
			std::cerr << "i3=" << i3 << " n3=" << n3 << "\n";
			exit(1);
		}
		return ListElt[idx];
	}
private:
	size_t n1, n2, n3;
	T *ListElt;
};