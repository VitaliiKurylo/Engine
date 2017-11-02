using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

namespace Engine.Editor.Operations
{
    [DebuggerDisplay("Size = {Size}; CurrentPosition = {CurrentPosition}")]
    class OperationList : List<Operation>
    {
        public OperationList()
        {
            Flush();
        }
        
        public bool CanRedo
        {
            get { return CurrentPosition < Count - 1; }
        }
        
        public bool CanUndo
        {
            get { return CurrentPosition >= 0; }
        }
        
        private int CurrentPosition { get; set; }
        
        public void Commit(Operation operation)
        {
            RemoveRange(CurrentPosition + 1, Count - CurrentPosition - 1);
            Add(operation);
            CurrentPosition++;
            Truncate();
        }
        
        public void Flush()
        {
            CurrentPosition = -1;
            Clear();
        }
        
        public Operation GetRedo()
        {
            return this[++CurrentPosition];
        }
        
        public Operation GetUndo()
        {
            return this[CurrentPosition--];
        }
        
        public IEnumerable<string> RedoOperations()
        {
            return this.Skip(CurrentPosition).Select(operation => operation.Caption);
        }
        
        public void Truncate(int limit = 0)
        {
            if (limit > 0)
            {
                if (Count > limit)
                {
                    var count = Count - limit;
                    RemoveRange(0, count);
                    CurrentPosition -= count;
                }
            }
        }
        
        public IEnumerable<string> UndoOperations()
        {
            return this.Take(CurrentPosition).Reverse().Select(operation => operation.Caption);
        }
    }
}
