@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @fib(i32 %n) {
entry:
  %retval = alloca i32, align 4
  %n2 = alloca i32, align 4
  store i32 %n, i32* %n2, align 4
  %0 = load i32, i32* %n2, align 4
  %cmp = icmp eq i32 %0, 0
  br i1 %cmp, label %label1, label %label2

label1:
  store i32 0, i32* %retval, align 4
  br label %rtn

label2:
  %1 = load i32, i32* %n2, align 4
  %cmp1 = icmp eq i32 %1, 1
  br i1 %cmp1, label %label3, label %label4

label3:
  store i32 1, i32* %retval, align 4
  br label %rtn

label4:
  %2 = load i32, i32* %n2, align 4
  %sub = sub nsw i32 %2, 2
  %call = call i32 @fib(i32 %sub)
  %3 = load i32, i32* %n2, align 4
  %sub4 = sub nsw i32 %3, 1
  %call5 = call i32 @fib(i32 %sub4)
  %add = add nsw i32 %call, %call5
  store i32 %add, i32* %retval, align 4
  br label %rtn

rtn:
  %4 = load i32, i32* %retval, align 4
  ret i32 %4
}

define i32 @main(i32 %argc, i8** %argv) {
entry:
  %retval = alloca i32, align 4
  %argc.addr = alloca i32, align 4
  %argv.addr = alloca i8**, align 8
  store i32 0, i32* %retval, align 4
  store i32 %argc, i32* %argc.addr, align 4
  store i8** %argv, i8*** %argv.addr, align 8
  %0 = load i8**, i8*** %argv.addr, align 8
  %arrayidx = getelementptr inbounds i8*, i8** %0, i64 1
  %1 = load i8*, i8** %arrayidx, align 8
  %call = call i32 @atoi(i8* %1) #3
  %call1 = call i32 @fib(i32 %call)
  %call2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %call1)
  ret i32 0
}

declare i32 @printf(i8*, ...)

declare i32 @atoi(i8*)
