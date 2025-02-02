class thread extends Thread {
    public void run() {
        try {
            Thread.sleep(2000);
        } catch (Exception e) {
            System.out.println("Error");
        }
        System.out
                .println("State of thread1 while it called join() method on thread2 -" + life_cycle.thread1.getState());
        try {
            Thread.sleep(200);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

class life_cycle implements Runnable {

    public static Thread thread1;
    public static life_cycle obj;

    public void run() {
        thread myThread = new thread();
        Thread thread2 = new Thread(myThread);
        System.out.println("State of thread2 after creating it - " + thread2.getState());
        thread2.start();
        System.out.println("State of thread2 after calling .start() method on it - " + thread2.getState());
        try {
            // moving thread1 to timed waiting state
            Thread.sleep(200);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println("State of thread2 after calling .sleep() method on it - " + thread2.getState());
        try {
            thread2.join();
        } catch (Exception e) {
            e.printStackTrace();
        }
        System.out.println("State of thread2 when it has finished it's execution - " + thread2.getState());
    }

    public static void main(String[] args) {
        obj = new life_cycle();
        thread1 = new Thread(obj);
        System.out.println("State of thread1 after creating it - " + thread1.getState());
        thread1.start();
        System.out.println("State of thread1 after calling .start() method on it - " + thread1.getState());
    }
}