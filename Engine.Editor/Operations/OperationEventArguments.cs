namespace Engine.Editor.Operations
{
    enum OperationType
    {
        Commit,
        Undo,
        Redo
    }

    class OperationEventArguments
    {
        public OperationType OperationType { get; }
        
        public OperationEventArguments(OperationType type)
        {
            OperationType = type;
        }
    }
}
