class AllOperation implements IOperation {
    private final int argCount;
    private final IOperation operation;

    public AllOperation(int argCount, IOperation operation) {
        this.argCount = argCount;
        this.operation = operation;
    }

    @Override
    public double Call(double... args) {
        if (args.length < argCount) {
            throw new IllegalArgumentException("недостаточно аргументов");
        }
        return operation.Call(args);
    }
}
