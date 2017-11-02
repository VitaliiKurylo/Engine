using Engine.Editor.Controls;
using Engine.Editor.Framework;
using PropertyChanged;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using Engine.Editor.Fluent;
using Engine.Editor.Services;
using Microsoft.Practices.ServiceLocation;

namespace Engine.Editor.Models
{
    struct ConnectorInfo
    {
        public Point Location { get; set; }

        public ConnectorOrientation Orientation { get; set; }

        public Point Position { get; set; }
    }

    [ImplementPropertyChanged()]
    class ConnectorModel : AbstractModel
    {
        private AbstractConnectorModel _sinkConnectorModel;
        private Point _sourceA;
        private Point _sourceB;
        private FullyCreatedConnectorModel _sourceConnectorModel;

        public ConnectorModel(IProjectDocument parent, FullyCreatedConnectorModel sourceConnectorModel, AbstractConnectorModel sinkConnectorModel)
            : base(parent)
        {
            SourceConnectorModel = sourceConnectorModel;
            SinkConnectorModel = sinkConnectorModel;
        }

        public ConnectorModel(FullyCreatedConnectorModel sourceConnectorModel, AbstractConnectorModel sinkConnectorModel)
            : this(sourceConnectorModel.Target.Parent, sourceConnectorModel, sinkConnectorModel)
        {
        }

        [Browsable(false)]
        public List<Point> ConnectionPoints { get; private set; }

        public bool IsFullConnection { get { return SinkConnectorModel is FullyCreatedConnectorModel; } }

        [Browsable(false)]
        public AbstractConnectorModel SinkConnectorModel
        {
            get { return _sinkConnectorModel; }
            set
            {
                _sinkConnectorModel.Not().Null(() =>
                {
                    _sinkConnectorModel = value;
                    if (IsFullConnection)
                    {
                        SourceB = PreparePoint((FullyCreatedConnectorModel)SinkConnectorModel);
                        WeakEventManager<INotifyPropertyChanged, PropertyChangedEventArgs>.RemoveHandler(((FullyCreatedConnectorModel)value).Target.Location, nameof(PropertyChanged),
                            ConnectorModel_LocationChanged);
                        WeakEventManager<INotifyPropertyChanged, PropertyChangedEventArgs>.AddHandler(((FullyCreatedConnectorModel)_sinkConnectorModel).Target.Location, nameof(PropertyChanged),
                            ConnectorModel_LocationChanged);
                    }
                    else
                    {
                        SourceB = ((PartiallyCreatedConnectorModel)SinkConnectorModel).CurrentLocation;
                    }
                    RaisePropertyChanged();
                });
            }
        }

        public Point SourceA
        {
            get { return _sourceA; }
            set { _sourceA = value; RaisePropertyChanged(); UpdateConnectionPoints(); }
        }
        
        public Point SourceB
        {
            get { return _sourceB; }
            set { _sourceB = value; RaisePropertyChanged(); UpdateConnectionPoints(); }
        }

        [Browsable(false)]
        public FullyCreatedConnectorModel SourceConnectorModel
        {
            get { return _sourceConnectorModel; }
            set
            {
                _sourceConnectorModel.Not().Null(() =>
                {
                    _sourceConnectorModel = value;
                    SourceA = PreparePoint(SourceConnectorModel);
                    WeakEventManager<INotifyPropertyChanged, PropertyChangedEventArgs>.RemoveHandler(value.Target.Location, nameof(PropertyChanged), ConnectorModel_LocationChanged);
                    WeakEventManager<INotifyPropertyChanged, PropertyChangedEventArgs>.AddHandler(_sourceConnectorModel.Target.Location, nameof(PropertyChanged), ConnectorModel_LocationChanged);
                    RaisePropertyChanged();
                });
            }
        }

        public ConnectorInfo GetConnectorInfo(ConnectorOrientation orientation, Point itemLocation, Point position)
        {
            return new ConnectorInfo
            {
                Orientation = orientation,
                Location = itemLocation,
                Position = position
            };
        }

        private void ConnectorModel_LocationChanged(object sender, PropertyChangedEventArgs eventArguments)
        {
            if (eventArguments.PropertyName == "X" || eventArguments.PropertyName == "Y")
            {
                SourceA = PreparePoint(SourceConnectorModel);
                if (IsFullConnection) SourceB = PreparePoint((FullyCreatedConnectorModel)SinkConnectorModel);
            }
        }

        private Point PreparePoint(FullyCreatedConnectorModel connectorModel)
        {
            dynamic connectorTargetLocation = connectorModel.Target.Location;
            dynamic connectorTargetSize = connectorModel.Target.Size;
            switch (connectorModel.Orientation)
            {
                case ConnectorOrientation.Left:
                    return new Point(connectorTargetLocation.X, connectorTargetLocation.Y + connectorTargetSize.Height / 2d);
                case ConnectorOrientation.Right:
                    return new Point(connectorTargetLocation.X + connectorTargetSize.Width, connectorTargetLocation.Y + connectorTargetSize.Height / 2d);
                case ConnectorOrientation.Top:
                    return new Point(connectorTargetLocation.X + connectorTargetSize.Width / 2d, connectorTargetLocation.Y);
                case ConnectorOrientation.Bottom:
                    return new Point(connectorTargetLocation.X + connectorTargetSize.Width / 2d, connectorTargetLocation.Y + connectorTargetSize.Height);
                default: return new Point(connectorTargetLocation.X, connectorTargetLocation.Y);
            }
        }

        private void UpdateConnectionPoints()
        {
            var connectorArea = new Rect(SourceA, SourceB);

            Size = connectorArea.Size;
            Location = connectorArea.Location;

            ConnectionPoints = new List<Point>
            {
                new Point(SourceA.X < SourceB.X ? 0d : ((dynamic)Size).Width, SourceA.Y < SourceB.Y ? 0d : ((dynamic)Size).Height),
                new Point(SourceA.X > SourceB.X ? 0d : ((dynamic)Size).Width, SourceA.Y > SourceB.Y ? 0d : ((dynamic)Size).Height)
            };

            var sourceModel = GetConnectorInfo(SourceConnectorModel.Orientation, ConnectionPoints[0], ConnectionPoints[0]);

            if (IsFullConnection)
            {
                var sinkModel = GetConnectorInfo(SinkConnectorModel.Orientation, ConnectionPoints[1], ConnectionPoints[1]);
                ConnectionPoints = ServiceLocator.Current.GetInstance<IConnectionPathService>().GetConnectionLine(sourceModel, sinkModel, true);
            }
            else
            {
                ConnectionPoints = ServiceLocator.Current.GetInstance<IConnectionPathService>().GetConnectionLine(sourceModel, ConnectionPoints[1], ConnectorOrientation.Left);
            }
        }
    }
}
