using Engine.Editor.Framework;
using Engine.Editor.Operations;
using PropertyChanged;
using System.ComponentModel;
using System.Windows;

namespace Engine.Editor.Models
{
    [ImplementPropertyChanged()]
    abstract class AbstractModel : AbstractSelectableModel
    {
        private Operationable<Point> _locationProperty;
        private Operationable<Size> _sizeProperty;
        
        protected AbstractModel()
            : this(default(IProjectDocument))
        {
        }
        
        protected AbstractModel(IProjectDocument projectDocument)
            : base(projectDocument)
        {
            _locationProperty = new Point();
            _sizeProperty = new Size(double.NaN, double.NaN);
        }

        [Category("General")]
        public string Name { get; set; }

        [Category("General")]
        public Operationable<Point> Location { get; set; }
        
        [Category("General")]
        public Operationable<Size> Size { get; set; }
    }
}
