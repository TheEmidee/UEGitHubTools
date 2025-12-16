#pragma once

#include "CoreMinimal.h"

#if !( ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3 ) 

#include "Delegates/IntegerSequence.h"

template < typename TRequest, typename TTuple, size_t... Is >
static auto MakeRequestWithTupleImpl( TTuple & tuple, const FString & cursor, TIntegerSequence< size_t, Is... > )
{
    typedef TGitHubToolsHttpRequestWrapper< TRequest > HttpRequestType;
    return MakeShared< HttpRequestType >(
        tuple.template Get< Is >()...,
        cursor );
}

template < typename TRequest, typename TTuple >
static auto MakeRequestWithTuple( TTuple & tuple, const FString & cursor )
{
    constexpr size_t TupleSize = TTupleArity< typename TRemoveReference< TTuple >::Type >::Value;
    return MakeRequestWithTupleImpl< TRequest >(
        tuple,
        cursor,
        TMakeIntegerSequence< size_t, TupleSize > {} );
}
#endif