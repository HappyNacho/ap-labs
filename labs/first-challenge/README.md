First Code Challenge
====================

This is the first challenge in your Advanced Programming Class.

1. Solve the [Length of a Nested Array](https://edabit.com/challenge/yXSTvCNen2DQHyrh6) challenge by using the programming language of your choice.
2. Below are some documentation sections that need to be updated. Those are required for the processor to build and execute your code.
3. Certainly, this problem is already solved in the web, so we recommend you to solve it by your own without consulting others solutions.
4. This is a exploratory excersice that will help the professor to adapt the labs and challenges for the course.
3. Have fun and come prepared on next class.

Build Requirements
------------------
```

This first challenge is completed using Java, so it can be compiled using Java 14, which was what I used on IntelliJ. Other Java versions may work but have not been tested.
```


How to build
------------
```
Compile as a standard Java program.
```


How to Run
----------
```
The input is created in the Main class, however, the actual counting functionality is in the 'count' method. At this moment it was not designed as an object oriented, as we can see from the static access.

The program will count all the elements on an arraylist of integer arrays, even if they are different sizes. (demo includes sizes of 4,3,2,1), it can count also single numbers but when added to the array list they have to be added by casting it to a new temp int array. Example:  test.add(new int[]{100});. Adding it as an int directly will now work.

```


General instructions
--------------------
1. Don't forget to sync first with the base [master](https://github.com/CodersSquad/ap-labs) branch.
2. Push your code to your personal fork.
3. Don't forget to update this `README.md` file with your project's `requirements`, `build` and `how to run` sections.
4. Have fun and don't forget the next section.


How to submit your work
=======================
```
GITHUB_USER=<your_github_user>  make submit
```
More details at: [Classify API](../../classify.md)
