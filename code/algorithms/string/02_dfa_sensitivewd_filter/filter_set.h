#ifndef _IM_FILTER_SET_H_
#define _IM_FILTER_SET_H_

#include <cstdint>

/** @file filter_set.h
  * @brief 
  * @author teng.qing
  * @date 2021/6/10
  */

class FilterSet {
private:
    int64_t *elements;

public:
    FilterSet() {
        int16_t len = 1 + (UINT16_MAX >> 6);
        elements = new int64_t[len];
    }

    void add(int no) {
        elements[no >> 6] |= (1L << (no & 63));
    }

    void add(int... no) {
        for (int currNo : no)
            elements[currNo >>> 6] |= (1L << (currNo & 63));
    }

    void remove(int no) {
        elements[(uint32_t)no >> 6] &= ~(1L << (no & 63));
    }

    /**
     *
     * @param no
     * @return true:添加成功	false:原已包含
     */
    bool addAndNotify(int no) {
        int eWordNum = no >>> 6;
        long oldElements = elements[eWordNum];
        elements[eWordNum] |= (1L << (no & 63));
        bool result = elements[eWordNum] != oldElements;
//		if (result)
//			size++;
        return result;
    }

    /**
     *
     * @param no
     * @return true:移除成功	false:原本就不包含
     */
    bool removeAndNotify(int no) {
        int eWordNum = no >>> 6;
        long oldElements = elements[eWordNum];
        elements[eWordNum] &= ~(1L << (no & 63));
        bool result = elements[eWordNum] != oldElements;
        return result;
    }

    bool contains(int no) {
        return (elements[no >>> 6] &(1L << (no & 63))) != 0;
    }

    bool containsAll(int... no) {
        if (no.length == 0)
            return true;
        for (int currNo : no)
            if ((elements[currNo >>> 6] &(1L << (currNo & 63))) == 0)
        return false;
        return true;
    }

    /**
     * 不如直接循环调用contains
     * @param no
     * @return
     */
    bool containsAll_ueslessWay(int... no) {
        long[]
        elements = new long[this.elements.length];
        for (int currNo : no) {
            elements[currNo >>> 6] |= (1L << (currNo & 63));
        }//这一步执行完跟循环调用contains差不多了

        for (int i = 0; i < elements.length; i++)
            if ((elements[i] & ~this.elements[i]) != 0)
                return false;
        return true;
    }

    /**
     * 目前没有去维护size，每次都是去计算size
     * @return
     */
    int size() {
        int size = 0;
        for (long element : elements)
            size += Long.bitCount(element);
        return size;
    }

//public static void main(String[] args) {
//        FilterSet oi = new FilterSet();
//        System.out.println(oi.elements.length);
//    }
};

#endif// _IM_FILTER_SET_H_
