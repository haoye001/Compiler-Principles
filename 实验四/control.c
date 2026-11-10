int main() {
    int i = 0, sum = 0, zero = 0;
    while (i < 6) {
        i = i + 1;
        if (i == 2) continue;
        if (i == 5) break;
        if (i != 3 && i > 0 || zero && 1 / zero) {
            sum = sum + i * 2;
        } else {
            sum = sum - 1;
        }
    }
    if (!zero || 1 / zero) {
        int i = 10;
        sum = sum + (+i + -2) / 2 % 3;
    }
    return sum;
}
