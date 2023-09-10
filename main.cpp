#include "RequestHandler.h"
#define ASYNC_THRESHOLD 1000000
//=============================================================================================
bool make_thread = false;
RequestHandler rh;
//---------------------------------------------------------------------------------------------
void quicksort(int* array, long left, long right, bool do_async) {
    if (left >= right) return;
    long left_bound = left;
    long right_bound = right;

    long middle = array[(left_bound + right_bound) / 2];

    do {
        while (array[left_bound] < middle) {
            left_bound++;
        }
        while (array[right_bound] > middle) {
            right_bound--;
        }

        if (left_bound <= right_bound) {
            std::swap(array[left_bound], array[right_bound]);
            left_bound++;
            right_bound--;
        }
    } while (left_bound <= right_bound);


    if (do_async && (right_bound - left) > ASYNC_THRESHOLD)
    {
            rh.pushRequest(quicksort, array, left, right_bound, do_async);
            rh.wait();
            quicksort(array, left_bound, right, false);
    }
    else {
        quicksort(array, left, right_bound, false);
        quicksort(array, left_bound, right, false);
     }
}

//==========================================================================================================================
void show(int* arr, size_t sz, uint8_t cols = 0) {
    uint8_t col = cols;
    for (size_t i = 0; i < sz; i++) {
        std::cout << arr[i] << ", ";
        if (cols && !--col) {
            std::cout << "\n";
            col = cols;
        }
    }
}
//==========================================================================================================================
void copy_array(const int* arr_src, int* arr_dst, size_t sz) {
    for (size_t i = 0; i < sz; i++)
        arr_dst[i] = arr_src[i];
}
//======================================================================================
int main(int argc, char** argv) {
	srand(0);
    long sz{ 100000000 };//100000000;
    long low{ 0 };
    int* arr = new int[sz];
    for (long i = 0; i < sz; i++) {
        arr[i] = rand() % 500000;
    }

    int* arr_as = new int[sz];
    copy_array(arr, arr_as, sz);

    bool show_array_contents = sz < 50;

    if (show_array_contents) {
        std::cout << "Source array:\n";
        show(arr_as, sz, 10);
    }

    
    std::cout << "---------------------------------------------------------------------------------------------------------\n";
    std::cout << "SYNC Quick Sort: array of " << sz << " items\n";
    std::cout << "=========================================================================================================\n";
    auto start = std::chrono::high_resolution_clock::now();
    quicksort(arr, 0, sz - 1,false);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Sync elapsed time: " << elapsed.count() << " sec" << std::endl;
    
    if (show_array_contents) {
        std::cout << "---------------------------------------------------------------------------------------------------------\n";
        std::cout << "Sorted array:\n";
        show(arr, sz, 10);
        std::cout << "---------------------------------------------------------------------------------------------------------\n";
    }
    
    make_thread = true;
    std::cout << "---------------------------------------------------------------------------------------------------------\n";
    std::cout << "Async Quick sort array of " << sz << " items\n";
    std::cout << "=========================================================================================================\n";
    start = std::chrono::high_resolution_clock::now();
    quicksort(arr_as, 0, sz - 1, make_thread);
    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;
    std::cout << "Async elapsed time: " << elapsed.count() << " sec" << std::endl;

    if (show_array_contents) {
        std::cout << "---------------------------------------------------------------------------------------------------------\n";
        std::cout << "Sorted array:\n";
        show(arr_as, sz, 10);
        std::cout << "---------------------------------------------------------------------------------------------------------\n";
    }
    

    delete[] arr;
    delete[] arr_as;

    std::cout << "Finished\n";

	return 0;

}
