package main

import "fmt"

// 算法描述（B站视频）：https://www.bilibili.com/video/BV1at411T75o?from=search&seid=17709611481602989795
// 1. 选定pivot中心轴
// 2. 将大于pivot的放右边
// 3. 将小于pivot的放左边
// 4. 分别对左右子序里重复前3步操作
func quickSort(arr []int, begin int, end int) {
	if begin >= end {
		return
	}
	// 分区
	pivot := partition(arr, begin, end)
	// 对左边递归快排
	quickSort(arr, begin, pivot-1)
	// 对右边递归快排
	quickSort(arr, pivot+1, end)
}

func partition(arr []int, begin int, end int) int {
	// 选取最后一个元素作为中轴的好处:
	// https://blog.csdn.net/longzuyuan/article/details/20633243
	pivot := end
	// 统计小于pivot的元素个数
	count := begin
	// 把所有小于pivot的元素都放到左侧
	for i := begin; i < end; i++ {
		if arr[i] < arr[pivot] {
			if arr[i] != arr[count] {
				swap(&arr[i], &arr[count])
			}
			count++
		}
	}
	// 用最后一个元素作为标杆，别忘记移到中间去
	swap(&arr[pivot], &arr[count])
	return count
}

func swap(a *int, b *int) {
	temp := *a
	*a = *b
	*b = temp
}

func main() {
	slice := []int{5, 8, 1, 45, 32, 6, 31, 58, 2}
	quickSort(slice, 0, len(slice)-1)
	for _, v := range slice {
		fmt.Println(v)
	}
}
