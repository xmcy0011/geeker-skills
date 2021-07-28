package main

import "fmt"

func mergeSort(arr []int, begin, end int) {
	if begin >= end {
		return
	}
	mid := (begin + end) / 2
	mergeSort(arr, begin, mid)
	mergeSort(arr, mid+1, end)
	merge(arr, begin, mid, end)
}

// begin - mid：有序
// mid - end: 有序
// 合并2个有序的数组
// 这里有一个很不好理解的地方：上面没看到有排序代码，为什么说是有序的？
// 这个视频：https://www.bilibili.com/video/BV1et411N7Ac?from=search&seid=12938757773760664991
// 里有补充说明，最小的组每个都是1个元素，所以自然也就是有序的
func merge(arr []int, begin int, mid int, end int) {
	temp := make([]int, end-begin+1)
	// 一开始指向2个头部
	i := begin
	j := mid + 1
	// 已经合并的个数
	k := 0

	// 3段式while
	// while: 比较2个数组的头部，小的取出来
	for ; i <= mid && j <= end; k++ {
		if arr[i] <= arr[j] {
			temp[k] = arr[i]
			i++
		} else {
			temp[k] = arr[j]
			j++
		}
	}

	// while: 有可能左边的有剩下，全部移进去
	for ; i <= mid; i++ {
		temp[k] = arr[i]
		k++
	}

	// while：也有可能右边的有剩下，全部移进去，和上面的情况是互斥的
	for ; j <= end; j++ {
		temp[k] = arr[j]
		k++
	}

	copy(arr[begin:end+1], temp)
}

func main() {
	slice := []int{1, 55, 23, 12, 7, 4, 64, 34, 7, 323}
	mergeSort(slice, 0, len(slice)-1)
	for _, v := range slice {
		fmt.Println(v)
	}
}
