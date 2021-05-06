#include"NumRos.hpp"
#include"NumCpp.hpp"
#include<iostream>

int main()
{
	int t1 = 1, t2 = 2;
	Ros::NdArray<int> b0{1, 2};
	std::cout << "b0 is:\n" << b0 << std::endl;
	Ros::NdArray<int> b1{{1, 2}, {3, 4}, {5, 6}};
	std::cout << "b1 is:\n" << b1 << std::endl;
	Ros::NdArray<int> bb1 = {{t1, t2}, {t1, t2}};
	std::cout << "bb1 is:\n" << bb1 << std::endl;
	std::cout << "b0(Ros::Slice(0, 1)) is:\n" << b0(Ros::Slice(0, 1)) << std::endl;
	//std::cout << b0(0);
	//std::cout << "b0({0, 1}) is:\n" << b0({0, 1}) << std::endl;
	//std::cout << b1({0,1}, {0,1});
	// Sorry to say that it's actually impossible to implement it because initialize_list can not be inferred.
	// It would be ambigious to do things like that, because it can also be construct of shape or slice or other things
	// Compiler cann't decide which one is best, or it actually have no priority bacause of rationality of each choice.
	Ros::NdArray<int> bbb1{{1, 2, 3}, {3, 4, 5}, {5, 6, 7}};
	std::cout << "bbb1 is:\n" << bbb1 << std::endl;;
	std::cout << "bbb1[2] is:\n" << bbb1[2] << std::endl;;
	std::cout << "bbb1(Ros::Slice(0, 2), Ros::Slice(0, 2), Ros::Slice(0, 2)) = 100 is:\n"
		<< (bbb1(Ros::Slice(0, 2), Ros::Slice(0, 2)) = 100) << std::endl;
	std::cout << "bbb1 is:\n" << bbb1 << std::endl;;
	auto s0 = Ros::Shape(1);
	auto s1 = Ros::Shape(1, 2);
	auto s2 = Ros::Shape(1, 2, 3);
	b1 = b1.reshape(6);
	std::cout << "b1 is:\n" << b1 << std::endl;
	b1 = b1.reshape(2, 3);
	std::cout << "b1 is:\n" << b1 << std::endl;
	b1 = b1.reshape({1, 2, 3});
	std::cout << "b1 is:\n" << b1 << std::endl;
	b1 = b1.reshape(1, 2, 3);
	std::cout << "b1 is:\n" << b1 << std::endl;
	Ros::NdArray<double> bbbbb1(16);
	std::cout << "bbbbb1 is:\n" << bbbbb1.reshape(2, 2, 2, 2) << std::endl;

	//b1.reshape(1, 2, 3);
	Ros::NdArray<double> b2(Ros::Shape({1,2,3}));
	std::cout << "b2 is:\n" << b2 << std::endl;;
	Ros::NdArray<double> bb2(1, 2, 3);
	std::cout << "bb2 is:\n" << bb2 << std::endl;;
	std::cout << b2.shape();
	std::cout << bb2.shape();

	//auto b6 = Ros::zeros<int>(3, 4);
	//It is not a good interface.
	auto b6 = Ros::zeros<double>({3,4});
	std::cout << "b6 is:\n" << b6 << std::endl;;
	auto bb6 = Ros::zeros<int>({3,4,5});
	std::cout << "bb6 is:\n" << bb6 << std::endl;;
	auto bbb6 = Ros::zeros<double>(Ros::Shape({3,4,5}));
	std::cout << "bbb6 is:\n" << bbb6 << std::endl;;
	auto bbbb6 = Ros::zeros<int>(3);
	std::cout << "bbbb6 is:\n" << bbbb6 << std::endl;;

	auto b7 = Ros::NdArray<int>(3, 4) = 0;
	std::cout << "b7 is:\n" << b7 << std::endl;;

	//auto b8 = Ros::ones<int>(3, 4);
	//It is not a good interface.
	auto b8 = Ros::ones<int>({3, 4});
	std::cout << "b8 is:\n" << b8 << std::endl;;
	auto bb8 = Ros::ones<double>({3,4,5});
	std::cout << "bb8 is:\n" << bb8 << std::endl;;
	auto bbb8 = Ros::ones<double>(Ros::Shape({3,4,5}));
	std::cout << "bbb8 is:\n" << bbb8 << std::endl;;
	auto bbbb8 = Ros::ones<double>(3);
	std::cout << "bbb8 is:\n" << bbb8 << std::endl;;

	auto b9 = Ros::NdArray<int>(3, 4) = 1;
	std::cout << "b9 is:\n" << b7 << std::endl;;
	auto d0 = b7 + b9;
	std::cout << "d0 is:\n" << d0 << std::endl;;
	auto d1 = b7 * b9;
	std::cout << "d1 is:\n" << d1 << std::endl;;
	auto d2 = b7 / b9;
	std::cout << "d2 is:\n" << d2 << std::endl;;
	auto d3 = b7 - b9;
	std::cout << "d3 is:\n" << d3 << std::endl;;

	auto b14 = Ros::random::rand<double>({6, 6});
	std::cout << "b14 is:\n" << b14 << std::endl;;
	auto bb14 = Ros::random::rand<double>(6, 6);
	std::cout << "bb14 is:\n" << bb14 << std::endl;;
	auto bbb14 = Ros::random::randFloat({6, 6}, -1, 1);
	std::cout << "bbb14 is:\n" << bbb14 << std::endl;;
	auto bbbb14 = Ros::random::randFloat(Ros::Shape(6, 6), -1, 1);
	std::cout << "bbbb14 is:\n" << bbbb14 << std::endl;
	std::cout << "bbbb14 + 1.0 is:\n" << bbbb14 + 1.0 << std::endl;
	std::cout << "bbbb14 - 1.0 is:\n" << bbbb14 - 1.0 << std::endl;
	std::cout << "bbbb14 * 2.0 is:\n" << bbbb14 * 2.0 << std::endl;
	std::cout << "bbbb14 / 2.0 is:\n" << bbbb14 / 2.0 << std::endl;
	std::cout << "bbbb14 + bbbb14 is:\n" << bbbb14 + bbbb14 << std::endl;
	std::cout << "bbbb14 - bbbb14 is:\n" << bbbb14 - bbbb14 << std::endl;
	std::cout << "bbbb14 * bbbb14 is:\n" << bbbb14 * bbbb14 << std::endl;
	std::cout << "bbbb14 / bbbb14 is:\n" << bbbb14 / bbbb14 << std::endl;
	std::cout << "dot(bbbb14, bbbb14) is:\n" << Ros::dot(bbbb14, bbbb14) << std::endl;

	auto b15 = Ros::random::randFloat({2, 3}, -1, 1);
	auto bb15 = b15.transpose();
	std::cout << "b15 is:\n" << b15 << std::endl;
	std::cout << "bb15 is:\n" << bb15 << std::endl;

	auto b16 = Ros::random::randFloat({2, 3}, -1, 1);
	auto bb16 = b16.swapaxes(0, -1);
	std::cout << "b16 is:\n" << b16 << std::endl;
	std::cout << "bb16 is:\n" << bb16 << std::endl;

	auto b17 = Ros::random::randFloat({3, 4}, -1, 1);
	//auto bb17 = flipud(b17);
	//auto bbb17 = fliplr(b17);
	auto bb17 = b17(Ros::Slice(2, -1, -1), b17.getSliceA(1));
	auto bbb17 = b17(b17.getSliceA(0), Ros::Slice(3, -1, -1));
	std::cout << "b17 is:\n" << b17 << std::endl;
	std::cout << "bb17 is:\n" << bb17 << std::endl;
	std::cout << "bbb17 is:\n" << bbb17 << std::endl;

	auto b18 = Ros::random::randFloat({3, 4}, -1, 1);
	//auto bb17 = flipud(b17);
	//auto bbb17 = fliplr(b17);
	auto m = max(b18);
	auto n = argmax(b18);
	std::cout << "b18 is:\n" << b18 << std::endl;
	std::cout << "max is:\n" << m << std::endl;
	std::cout << "argmax is:\n" << n << std::endl;
	std::cout << "argmax's value is:\n" << b18[n] << std::endl;

	std::cout << (b17(Ros::Slice(1, 2)) += b18(Ros::Slice(1, 2))) << std::endl;

	for(auto& value : b18)
		std::cout << value;
	system("pause");
	return 0;
}
