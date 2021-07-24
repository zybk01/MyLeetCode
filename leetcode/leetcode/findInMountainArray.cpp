#include "stdafx.h"
#include<vector>
#include<iostream>


using namespace std;



 // This is the MountainArray's API interface.
 // You should not implement it, or speculate about its implementation
class MountainArray {
   public:
     int get(int index);
     int length();
 };


class Solution {
public:
	int findInMountainArray(int target, MountainArray &mountainArr) {
		int mark = mountainArr.length();
		if (mark==1)
		{
			return target == mountainArr.get(0)?0:-1;
		}
		int l = 0;
		int r = mark;
		while (l<r)
		{
			int temp = (l + r) / 2;

			
			if (mountainArr.get(temp +1) > mountainArr.get(temp)) {
				if (target == mountainArr.get(temp))
				{
					return temp;
				}
				if (target<mountainArr.get(temp))
				{
					r = temp;
				}
				else
				{
					l = temp+1;
				}
			}
			else
			{
				if (target<mountainArr.get(temp))
				{
					mark = temp+1;
				}
					r = temp;
			}

		}
		if (target == mountainArr.get(l))
		{
			return l;
		}
		l = mark;
		r= mountainArr.length();
		while (l<r)
		{
			int temp = (l + r) / 2;
			if (target == mountainArr.get(temp))
			{
				return temp;
			}

			if (mountainArr.get(temp + 1) <= mountainArr.get(temp)) {
				if (target<mountainArr.get(temp))
				{
					l = temp+1;
				}
				else
				{
					r = temp;
				}
			}
		}

		if (target == mountainArr.get(l))
		{
			return l;
		}
		return -1;

	}
};