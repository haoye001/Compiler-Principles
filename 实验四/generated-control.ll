define i32 @main() {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 0, i32* %2, align 4
  store i32 0, i32* %3, align 4
  store i32 0, i32* %4, align 4
  br label %6
6:
  %7 = load i32, i32* %2, align 4
  %8 = icmp slt i32 %7, 6
  br i1 %8, label %9, label %41
9:
  %10 = load i32, i32* %2, align 4
  %11 = add nsw i32 %10, 1
  store i32 %11, i32* %2, align 4
  %12 = load i32, i32* %2, align 4
  %13 = icmp eq i32 %12, 2
  br i1 %13, label %14, label %15
14:
  br label %6
15:
  %16 = load i32, i32* %2, align 4
  %17 = icmp eq i32 %16, 5
  br i1 %17, label %18, label %19
18:
  br label %41
19:
  %20 = load i32, i32* %2, align 4
  %21 = icmp ne i32 %20, 3
  br i1 %21, label %22, label %25
22:
  %23 = load i32, i32* %2, align 4
  %24 = icmp sgt i32 %23, 0
  br i1 %24, label %32, label %25
25:
  %26 = load i32, i32* %4, align 4
  %27 = icmp ne i32 %26, 0
  br i1 %27, label %28, label %37
28:
  %29 = load i32, i32* %4, align 4
  %30 = sdiv i32 1, %29
  %31 = icmp ne i32 %30, 0
  br i1 %31, label %32, label %37
32:
  %33 = load i32, i32* %3, align 4
  %34 = load i32, i32* %2, align 4
  %35 = mul nsw i32 %34, 2
  %36 = add nsw i32 %33, %35
  store i32 %36, i32* %3, align 4
  br label %40
37:
  %38 = load i32, i32* %3, align 4
  %39 = sub nsw i32 %38, 1
  store i32 %39, i32* %3, align 4
  br label %40
40:
  br label %6
41:
  %42 = load i32, i32* %4, align 4
  %43 = icmp ne i32 %42, 0
  br i1 %43, label %44, label %48
44:
  %45 = load i32, i32* %4, align 4
  %46 = sdiv i32 1, %45
  %47 = icmp ne i32 %46, 0
  br i1 %47, label %48, label %55
48:
  store i32 10, i32* %5, align 4
  %49 = load i32, i32* %3, align 4
  %50 = load i32, i32* %5, align 4
  %51 = add nsw i32 %50, -2
  %52 = sdiv i32 %51, 2
  %53 = srem i32 %52, 3
  %54 = add nsw i32 %49, %53
  store i32 %54, i32* %3, align 4
  br label %55
55:
  %56 = load i32, i32* %3, align 4
  ret i32 %56
}
