class Main {
  main(): Int {
	let i : Int <- 1, sum : Int <- 0 in {
		while i < 101 loop { sum <- sum + i; i <- i + 1; } pool;
		sum;
	}
  };
};
