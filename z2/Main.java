import java.util.*;

public class Main {
    private static final Dictionary<String, IOperation> operations = new Hashtable<>();
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        initOperations();
        while (true) {
            System.out.print("op: ");
            String op = scanner.nextLine().trim().toLowerCase();

            IOperation operation = operations.get(op);
            if (operation == null) {
                System.out.println("неизвестная операция");
                continue;
            }

            System.out.print("args: ");
            String[] arg = scanner.nextLine().split("\\s+");
            double[] numbers = Arrays.stream(arg).mapToDouble(Double::parseDouble).toArray();

            try {
                double result = operation.Call(numbers);
                System.out.println("результат: " + result);
            } catch (IllegalArgumentException e) {
                System.out.println("ошибка: " + e.getMessage());
            }
        }
    }


    private static void initOperations() {
        operations.put("+", new AllOperation(2, arg -> arg[0] + arg[1]));
        operations.put("-", new AllOperation(2, arg -> arg[0] - arg[1]));
        operations.put("*", new AllOperation(2, arg -> arg[0] * arg[1]));
        operations.put("/", new AllOperation(2, arg -> {
            if (arg[1] == 0) throw new ArithmeticException("деленить на ноль нельзя");
            return arg[0] / arg[1];
        }));

        operations.put("sin", new AllOperation(1, arg -> Math.sin(arg[0])));
        operations.put("cos", new AllOperation(1, arg -> Math.cos(arg[0])));
        operations.put("sqrt", new AllOperation(1, arg -> Math.sqrt(arg[0])));
        operations.put("abs", new AllOperation(1, arg -> Math.abs(arg[0])));
    }
}

