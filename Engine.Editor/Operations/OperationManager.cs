using System;

namespace Engine.Editor.Operations
{
    static class OperationManager
    {
        private static OperationList _operationList;
        
        static OperationManager()
        {
            _operationList = new OperationList();
            _operationList.Flush();
        }
        
        public static bool CanRedo
        {
            get { return _operationList.CanRedo; }
        }
        
        public static bool CanUndo
        {
            get { return _operationList.CanUndo; }
        }
        
        public static bool IsCommandStarted
        {
            get { return ReferenceEquals(CurrentOperation, null) == false; }
        }

        public static Operation CurrentOperation { get; private set; }
        
        public static event EventHandler<OperationEventArguments> OperationCompleted;
        
        public static void Cancel()
        {
            AssertOperation();
            foreach (var member in CurrentOperation.Keys)
                member.OnUndo(CurrentOperation[member]);

            CurrentOperation = null;
        }
        
        public static void Commit()
        {
            AssertOperation();
            foreach (var member in CurrentOperation.Keys)
                member.OnCommit(CurrentOperation[member]);

            _operationList.Commit(CurrentOperation);
            CurrentOperation = null;
            RaiseOperationComplete(OperationType.Commit);
        }
        
        public static void Redo()
        {
            AssertNoOperation();
            if (CanRedo)
            {
                var operation = _operationList.GetRedo();
                foreach (var member in operation.Keys)
                    member.OnRedo(operation[member]);

                RaiseOperationComplete(OperationType.Redo);
            }
        }
        
        public static IDisposable Start(string commandCaption)
        {
            AssertNoOperation();
            CurrentOperation = new Operation(commandCaption);
            return CurrentOperation;
        }
        
        public static bool TryActivateOperationList(OperationList list)
        {
            if (ReferenceEquals(list, null))
                return false;

            _operationList = list;
            return true;
        }
        
        public static void Undo()
        {
            AssertNoOperation();
            if (CanUndo)
            {
                var operation = _operationList.GetUndo();
                foreach (var member in operation.Keys)
                    member.OnUndo(operation[member]);

                RaiseOperationComplete(OperationType.Undo);
            }
        }
        
        internal static void AssertNoOperation()
        {
            if (ReferenceEquals(CurrentOperation, null) == false)
            {
                throw new InvalidOperationException("Previous command is not completed. Use OperationManager.Commit() to complete current command.");
            }
        }
        
        internal static void AssertOperation()
        {
            if (ReferenceEquals(CurrentOperation, null))
            {
                throw new InvalidOperationException("Command is not started. Use method OperationManager.Start().");
            }
        }
        
        private static void RaiseOperationComplete(OperationType type)
        {
            if (ReferenceEquals(OperationCompleted, null) == false)
            {
                OperationCompleted(_operationList, new OperationEventArguments(type));
            }
        }
    }
}
