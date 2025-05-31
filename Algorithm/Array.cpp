bool isDuplicatedNumber(int *array, int n) {
    if (array == nullptr) {
        return false;
    }

    int i, temp;
    for (int i = 0; i < n; ++i) {
        while (array[i] != i) {
            if (array[array[i]] == array[i]) {
                return true;
            }
            temp = array[array[i]];
            array[array[i]] = temp;
            array[i] = temp;
        }
    }

    return false;
}

