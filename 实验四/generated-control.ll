define i32 @main() {
bb0:
  %t1 = alloca i32, align 4
  %t2 = alloca i32, align 4
  %t3 = alloca i32, align 4
  %t4 = alloca i32, align 4
  store i32 0, ptr %t1, align 4
  store i32 0, ptr %t2, align 4
  store i32 0, ptr %t3, align 4
  br label %bb1
bb1:
  %t5 = load i32, ptr %t1, align 4
  %t6 = icmp slt i32 %t5, 6
  br i1 %t6, label %bb2, label %bb3
bb2:
  %t7 = load i32, ptr %t1, align 4
  %t8 = add i32 %t7, 1
  store i32 %t8, ptr %t1, align 4
  %t9 = load i32, ptr %t1, align 4
  %t10 = icmp eq i32 %t9, 2
  br i1 %t10, label %bb4, label %bb5
bb4:
  br label %bb1
bb5:
  %t11 = load i32, ptr %t1, align 4
  %t12 = icmp eq i32 %t11, 5
  br i1 %t12, label %bb6, label %bb7
bb6:
  br label %bb3
bb7:
  %t13 = load i32, ptr %t1, align 4
  %t14 = icmp ne i32 %t13, 3
  br i1 %t14, label %bb11, label %bb12
bb11:
  %t15 = load i32, ptr %t1, align 4
  %t16 = icmp sgt i32 %t15, 0
  br label %bb12
bb12:
  %t17 = phi i1 [ false, %bb7 ], [ %t16, %bb11 ]
  br i1 %t17, label %bb14, label %bb13
bb13:
  %t18 = load i32, ptr %t3, align 4
  %t19 = icmp ne i32 %t18, 0
  br i1 %t19, label %bb15, label %bb16
bb15:
  %t20 = load i32, ptr %t3, align 4
  %t21 = sdiv i32 1, %t20
  %t22 = icmp ne i32 %t21, 0
  br label %bb16
bb16:
  %t23 = phi i1 [ false, %bb13 ], [ %t22, %bb15 ]
  br label %bb14
bb14:
  %t24 = phi i1 [ true, %bb12 ], [ %t23, %bb16 ]
  br i1 %t24, label %bb8, label %bb10
bb8:
  %t25 = load i32, ptr %t2, align 4
  %t26 = load i32, ptr %t1, align 4
  %t27 = mul i32 %t26, 2
  %t28 = add i32 %t25, %t27
  store i32 %t28, ptr %t2, align 4
  br label %bb9
bb10:
  %t29 = load i32, ptr %t2, align 4
  %t30 = sub i32 %t29, 1
  store i32 %t30, ptr %t2, align 4
  br label %bb9
bb9:
  br label %bb1
bb3:
  %t31 = load i32, ptr %t3, align 4
  %t32 = icmp ne i32 %t31, 0
  %t33 = xor i1 %t32, true
  br i1 %t33, label %bb20, label %bb19
bb19:
  %t34 = load i32, ptr %t3, align 4
  %t35 = sdiv i32 1, %t34
  %t36 = icmp ne i32 %t35, 0
  br label %bb20
bb20:
  %t37 = phi i1 [ true, %bb3 ], [ %t36, %bb19 ]
  br i1 %t37, label %bb17, label %bb18
bb17:
  store i32 10, ptr %t4, align 4
  %t38 = load i32, ptr %t2, align 4
  %t39 = load i32, ptr %t4, align 4
  %t40 = sub i32 0, 2
  %t41 = add i32 %t39, %t40
  %t42 = sdiv i32 %t41, 2
  %t43 = srem i32 %t42, 3
  %t44 = add i32 %t38, %t43
  store i32 %t44, ptr %t2, align 4
  br label %bb18
bb18:
  %t45 = load i32, ptr %t2, align 4
  ret i32 %t45
}
