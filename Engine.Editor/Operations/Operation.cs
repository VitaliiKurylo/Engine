using System;
using System.Collections.Generic;
using Engine.Editor.Fluent;
using System.Diagnostics;

namespace Engine.Editor.Operations
{
    class Operation : Dictionary<IOperationMember, object>, IDisposable
    {
        public string Caption { get; }
        
        public Operation(string caption)
        {
            Caption = caption;
        }

        #region IDisposable Members
        
        void IDisposable.Dispose()
        {
            OperationManager.CurrentOperation.Not().Null(() =>
            {
                if (ReferenceEquals(OperationManager.CurrentOperation, this))
                    OperationManager.Cancel();
                else Debug.Fail("There was another command within disposed command");
            });
        }

        #endregion IDisposable Members
    }
}
