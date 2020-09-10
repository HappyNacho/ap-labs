import java.util.ArrayList;

public class Main {

    public static void main(String[] args) {

        ArrayList<int[]> test = new ArrayList<>();
        int[] test2 = {1,2,3,4};
        int[] test3 = {5,6,7};
        int[] test4 = {8,9};
        int[] test5 = {10};
        test.add(test2);
        test.add(test3);
        test.add(test4);
        test.add(test5);
        test.add(new int[]{100});
        int answer= count(test);
        System.out.println(answer);

    }

    static int count(ArrayList myArray){

        if(myArray.isEmpty()){
            return 0;
        }
        else{
            int count = 0;
            int size = myArray.size();
            for(int i=0; i<size;i++){
                int[] temp = (int[]) myArray.get(i);
                count = count+ temp.length;
            }
            return count;
        }
    }
}
