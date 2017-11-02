using System;
using System.Diagnostics;
using System.Linq.Expressions;

namespace Engine.Editor.Operations
{
    class OperationProperty<TValue> : IOperationMember
    {
        private readonly INotifyOperationCompleted _parent;
        private readonly string _propertyName;
        private TValue _propertyValue;

        public OperationProperty(Expression<Func<INotifyOperationCompleted, TValue>> action)
            : this(action, default(TValue))
        {
        }

        public OperationProperty(Expression<Func<INotifyOperationCompleted, TValue>> action, TValue defaultValue)
            : this(Expression.Lambda<Func<INotifyOperationCompleted>>(((MemberExpression)action.Body).Expression).Compile()(),
                  ((MemberExpression)action.Body).Member.Name)
        {
            _propertyValue = defaultValue;
        }

        public OperationProperty(INotifyOperationCompleted parent, string propertyName)
        {
            _parent = parent;
            _propertyName = propertyName;
        }

        public TValue Value
        {
            get { return _propertyValue; }
            set
            {
                if (ReferenceEquals(OperationManager.CurrentOperation, null) == false)
                {
                    if (OperationManager.CurrentOperation.ContainsKey(this) == false)
                        OperationManager.CurrentOperation[this] = new OperationChange<TValue> { OldState = _propertyValue };
                }
                _propertyValue = value;
            }
        }

        #region IOperationMember Members

        void IOperationMember.OnCommit(object change)
        {
            Debug.Assert(ReferenceEquals(change, null) == false);
            ((OperationChange<TValue>)change).NewState = _propertyValue;
        }

        void IOperationMember.OnRedo(object change)
        {
            Debug.Assert(ReferenceEquals(change, null) == false);
            _propertyValue = ((OperationChange<TValue>)change).NewState;
            _parent.RaisePropertyChanged(_propertyName);
        }

        void IOperationMember.OnUndo(object change)
        {
            Debug.Assert(ReferenceEquals(change, null) == false);
            _propertyValue = ((OperationChange<TValue>)change).OldState;
            _parent.RaisePropertyChanged(_propertyName);
        }

        #endregion IOperationMember Members

        private class OperationChange<TState>
        {
            public TState NewState;

            public TState OldState;
        }
    }
}
