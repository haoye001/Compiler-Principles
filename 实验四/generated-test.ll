define i32 @main() {
bb0:
  %t1 = alloca i32, align 4
  store i32 10, ptr %t1, align 4
  %t2 = load i32, ptr %t1, align 4
  %t3 = srem i32 %t2, 3
  ret i32 %t3
}
