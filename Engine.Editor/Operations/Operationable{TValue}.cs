using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Dynamic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace Engine.Editor.Operations
{
    sealed class Operationable<TValue> : DynamicObject, INotifyOperationCompleted
    {
        private readonly Dictionary<string, object> _operationProperties;

        private TValue _object;

        public Operationable(TValue @object)
        {
            _object = @object;
            _operationProperties = new Dictionary<string, object>();
        }

        private PropertyInfo GetPropertyInfo(string propertyName)
        {
            return typeof(TValue).GetProperties().FirstOrDefault((element) => element.Name == propertyName);
        }

        public override bool TryGetMember(GetMemberBinder binder, out object result)
        {
            return _operationProperties.TryGetValue(binder.Name, out result);
        }

        public override bool TrySetMember(SetMemberBinder binder, object value)
        {
            var operationProperty = GetPropertyInfo(binder.Name);
            if (operationProperty == null) return false;

            var operationPropertyType = typeof(OperationProperty<>).MakeGenericType(operationProperty.PropertyType);

            _operationProperties.Add(operationProperty.Name, Activator.CreateInstance(operationPropertyType, this, operationProperty.Name));

            return true;
        }

        public override bool TryConvert(ConvertBinder binder, out object result)
        {
            if (binder.Type == typeof(TValue) || binder.Type.IsAssignableFrom(typeof(TValue)))
            {
                result = _object;
                return true;
            }

            return base.TryConvert(binder, out result);
        }

        public override IEnumerable<string> GetDynamicMemberNames()
        {
            return _operationProperties.Keys;
        }

        public static implicit operator Operationable<TValue>(TValue @object)
        {
            return new Operationable<TValue>(@object);
        }

        public static explicit operator TValue(Operationable<TValue> @object)
        {
            return (TValue)(dynamic)@object;
        }

        #region INotifyOperationCompleted Members

        public event PropertyChangedEventHandler PropertyChanged;

        public void RaisePropertyChanged([CallerMemberName] string propertyName = null)
        {
            if (ReferenceEquals(PropertyChanged, null) == false)
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion INotifyOperationCompleted Members
    }
}
