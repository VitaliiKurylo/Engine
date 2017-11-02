using Engine.Editor.Commands;
using Engine.Editor.Framework;
using Engine.Editor.Operations;
using PropertyChanged;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Windows.Input;

namespace Engine.Editor.Models
{
    [ImplementPropertyChanged()]
    abstract class AbstractSelectableModel : OperationObject, ISelectable, IGroupable
    {
        private ICommand _selectCommand;

        protected AbstractSelectableModel(IProjectDocument projectDocument)
        {
            Parent = projectDocument;
            Id = Guid.NewGuid();
        }

        [Browsable(false)]
        public IProjectDocument Parent { get; }

        [Browsable(false)]
        public bool IsSelected { get; set; }

        [Browsable(false)]
        public ICommand SelectCommand { get { return _selectCommand ?? (_selectCommand = new RelayCommand(ExecuteSelectCommand)); } }

        [Browsable(false)]
        public List<ISelectable> SelectedElements { get { return Parent.SelectedElements.OfType<ISelectable>().ToList(); } }

        [Browsable(false)]
        public Guid Id { get; }

        [Browsable(false)]
        public Guid ParentId { get; set; }

        [Browsable(false)]
        public bool IsGrouped { get; set; }
        
        private void ExecuteSelectCommand(object parameter)
        {
            if ((bool)parameter)
                Parent.SelectedElements.ForEach((element) => element.IsSelected = false);

            IsSelected = !IsSelected;
        }
    }
}
