import graphene
from graphene import relay
from graphene_sqlalchemy import SQLAlchemyObjectType, SQLAlchemyConnectionField
from models import db_session, Telemetry as TelemetryModel

class Telemetry(SQLAlchemyObjectType):
    class Meta:
        model = TelemetryModel
        interfaces = (relay.Node, )

class TelemConnectionField(SQLAlchemyConnectionField):
    RELAY_ARGS = ['first', 'last', 'before', 'after']

    @classmethod
    def get_query(cls, model, context, info, args):
        query = super(TelemConnectionField, cls).get_query(model, context, info, args)
        for field, value in args.items():
            if field not in cls.RELAY_ARGS:
                query = query.filter(getattr(model, field) == value)
        return query

class Query(graphene.ObjectType):
    node = relay.Node.Field()
    all_telemetry = SQLAlchemyConnectionField(Telemetry)
    telem_by_subsys = TelemConnectionField(Telemetry,
                                           subsystem=graphene.String(),
                                           param=graphene.String())


schema = graphene.Schema(query=Query)
