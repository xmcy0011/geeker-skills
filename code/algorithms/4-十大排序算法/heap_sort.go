package main

import "fmt"

func swap(tree []int, i, j int) {
	temp := tree[i]
	tree[i] = tree[j]
	tree[j] = temp
}

func heapify(tree []int, length, i int) {
	if i >= length {
		return
	}

	// 完全二叉树，可以用一维数组表示，此时可以计算各个节点的位置
	c1 := 2*i + 1
	c2 := 2*i + 2
	max := i

	// 2个子节点比父节点大，记录，注意C1和C2可能越界
	if c1 < length && tree[c1] > tree[max] {
		max = c1
	}

	if c2 < length && tree[c2] > tree[max] {
		max = c2
	}

	if max != i {
		swap(tree, max, i)
		// 继续往上找
		heapify(tree, length, max)
	}
}

func buildHeap(tree []int, length int) {
	lastNode := length - 1
	parent := (lastNode - 1) / 2
	for i := parent; i >= 0; i-- {
		heapify(tree, length, i)
	}
}

// 这颗树如下：
//     4
//  10   3
// 5  1 2
func testHeapify() {
	treeSlice := []int{4, 10, 3, 5, 1, 2}
	length := len(treeSlice)
	heapify(treeSlice, length, 0)

	for _, v := range treeSlice {
		fmt.Println(v)
	}

}

func heapSort(arr []int, length int) {
	buildHeap(arr, length)
	for i := length - 1; i >= 0; i-- {
		swap(arr, i, 0)
		heapify(arr, i, 0)
	}
}

func main() {
	//testHeapify()
	slice := []int{4, 10, 3, 5, 1, 2}
	length := len(slice)
	heapSort(slice, length)

	for _, v := range slice {
		fmt.Println(v)
	}
}
