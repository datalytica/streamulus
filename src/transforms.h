//
//  transforms.h
//
// Streamulus Copyright (c) 2012 Irit Katriel. All rights reserved.
//
// This file is part of Streamulus.
// 
// Streamulus is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Streamulus is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Streamulus.  If not, see <http://www.gnu.org/licenses/>.
//

#pragma once

// Transforms for streamulus grammar

#include "engine.h"
#include "stream.h"
#include "strop.h"
#include "strop_func.h"
#include "engine_api.h"

#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/include/algorithm.hpp>


namespace streamulus
{
    
    using namespace boost;
        
    template<typename SharedPtrType>
    struct BaseType;
    
    template<typename Type>
    struct BaseType<boost::shared_ptr<Type> >
    {
        typedef Type type;
    };
    
    
    template<typename Sig>
    struct MakeStropPtrType
    {  
        typedef boost::shared_ptr<Strop<Sig> > type;    
    };
    
    
    template<typename StropType>
    struct StropReturnType;
    
    template<typename StropType>
    struct StropReturnType<const boost::shared_ptr<StropType> >
    {
        typedef typename StropType::result_type type;
    };
    
    template<typename StropType>
    struct StropReturnType<const boost::shared_ptr<StropType>& >
    {
        typedef typename StropType::result_type type;
    };
    
    
    struct AddStropToGraph : proto::callable  
    {
        template<class Sig> struct result;
        
        template<class This, class T, class State>
        struct result<This(T,State)>
        {
            typedef T type;
        };
        
        template<typename StropType>
        const StropType operator()(StropType strop, EngineApi* engine)
        {
            std::cout << "AddStropToGraph: strop=" << strop << std::endl;
            if (!strop->GraphIsSet())
            {
                std::cout << "AddStropToGraph::operator()" << std::endl;
                engine->AddVertexToGraph(strop);
                engine->AddSource(strop);
            }
            std::cout << "returning " << strop->Descriptor() << std::endl;
            return strop;
        }
    };
    
    
    struct generic_func : proto::callable
    {
        template<typename Sig> struct result;
        
        // Arity = 1
        template<typename F, 
        typename Arg1Strop, 
        typename State>
        struct result<generic_func(F&,Arg1Strop,State)>
        {
            typedef typename StropReturnType<const Arg1Strop>::type Arg1Type;
            typedef typename F::template result<F(Arg1Type)>::type R; 
            typedef const boost::shared_ptr<Strop<R(Arg1Type)> > type;         
        };
        
        template<typename F, 
        typename Arg1Strop, 
        typename State>
        typename result<generic_func(F&,Arg1Strop,State)>::type
        operator()(const F& f,
                   const Arg1Strop arg1, 
                   State engine)
        {   
            std::cout << "generic_func" << std::endl;
            typedef result<generic_func(F&,Arg1Strop,State)> Result;        
            typedef Func1<F
            ,typename Result::Arg1Type
            > FuncStropType; 
            
            boost::shared_ptr<FuncStropType> funcStropPtr(new FuncStropType()); 
            
            typename BaseType<Arg1Strop>::type::OutputStreamPtr arg1Stream(arg1->MakeOutputStream());
            funcStropPtr->SetInputs(boost::fusion::make_vector(arg1Stream));
            
            engine->AddVertexToGraph(funcStropPtr);
            engine->AddEdgeToGraph(arg1->Descriptor(), funcStropPtr->Descriptor(), arg1Stream);
            return funcStropPtr;
        }

        // Arity = 2
        template<typename F, typename Arg1Strop, typename Arg2Strop, typename State>
        struct result<generic_func(F&,Arg1Strop,Arg2Strop,State)>
        {
            typedef typename StropReturnType<const Arg1Strop>::type Arg1Type;
            typedef typename StropReturnType<const Arg2Strop>::type Arg2Type;
            typedef typename F::template result<F(Arg1Type,Arg2Type)>::type R; 
            typedef const boost::shared_ptr<Strop<R(Arg1Type,Arg2Type)> > type;         
        };
        
        template<typename F, typename Arg1Strop, typename Arg2Strop, typename State>
        typename result<generic_func(F&,Arg1Strop,Arg2Strop, State)>::type
        operator()(const F& f,const Arg1Strop arg1, const Arg2Strop arg2, State engine)
        {   
            std::cout << "generic_func" << std::endl;
            typedef result<generic_func(F&,Arg1Strop,Arg2Strop, State)> Result;        
            typedef Func2<F, typename Result::Arg1Type, typename Result::Arg2Type> FuncStropType; 
            boost::shared_ptr<FuncStropType> funcStropPtr(new FuncStropType()); 
            
            typename BaseType<Arg1Strop>::type::OutputStreamPtr arg1Stream(arg1->MakeOutputStream());
            typename BaseType<Arg2Strop>::type::OutputStreamPtr arg2Stream(arg2->MakeOutputStream());
            funcStropPtr->SetInputs(boost::fusion::make_vector(arg1Stream, arg2Stream));
            
            engine->AddVertexToGraph(funcStropPtr);
            engine->AddEdgeToGraph(arg1->Descriptor(), funcStropPtr->Descriptor(), arg1Stream);
            engine->AddEdgeToGraph(arg2->Descriptor(), funcStropPtr->Descriptor(), arg2Stream);
            return funcStropPtr;
        }

        // Arity = 3
        template<typename F, 
        typename Arg1Strop, 
        typename Arg2Strop, 
        typename Arg3Strop, 
        typename State>
        struct result<generic_func(F&,Arg1Strop,Arg2Strop,Arg3Strop,State)>
        {
            typedef typename StropReturnType<const Arg1Strop>::type Arg1Type;
            typedef typename StropReturnType<const Arg2Strop>::type Arg2Type;
            typedef typename StropReturnType<const Arg3Strop>::type Arg3Type;
            typedef typename F::template result<F(Arg1Type,Arg2Type,Arg3Type)>::type R; 
            typedef const boost::shared_ptr<Strop<R(Arg1Type,Arg2Type,Arg3Type)> > type;         
        };
        
        template<typename F, 
        typename Arg1Strop, 
        typename Arg2Strop, 
        typename Arg3Strop, 
        typename State>
        typename result<generic_func(F&,Arg1Strop,Arg2Strop,Arg3Strop,State)>::type
        operator()(const F& f,
                   const Arg1Strop arg1, 
                   const Arg2Strop arg2, 
                   const Arg3Strop arg3, 
                   State engine)
        {   
            std::cout << "generic_func" << std::endl;
            typedef result<generic_func(F&,Arg1Strop,Arg2Strop,Arg3Strop,State)> Result;        
            typedef Func3<F
            ,typename Result::Arg1Type
            ,typename Result::Arg2Type
            ,typename Result::Arg3Type
            > FuncStropType; 
            
            boost::shared_ptr<FuncStropType> funcStropPtr(new FuncStropType()); 
            
            typename BaseType<Arg1Strop>::type::OutputStreamPtr arg1Stream(arg1->MakeOutputStream());
            typename BaseType<Arg2Strop>::type::OutputStreamPtr arg2Stream(arg2->MakeOutputStream());
            typename BaseType<Arg2Strop>::type::OutputStreamPtr arg3Stream(arg3->MakeOutputStream());
            funcStropPtr->SetInputs(boost::fusion::make_vector(arg1Stream, arg2Stream, arg3Stream));
            
            engine->AddVertexToGraph(funcStropPtr);
            engine->AddEdgeToGraph(arg1->Descriptor(), funcStropPtr->Descriptor(), arg1Stream);
            engine->AddEdgeToGraph(arg2->Descriptor(), funcStropPtr->Descriptor(), arg2Stream);
            engine->AddEdgeToGraph(arg3->Descriptor(), funcStropPtr->Descriptor(), arg3Stream);
            return funcStropPtr;
        }
        
        // Arity = 4
        template<typename F, 
        typename Arg1Strop, 
        typename Arg2Strop, 
        typename Arg3Strop, 
        typename Arg4Strop, 
        typename State>
        struct result<generic_func(F&,Arg1Strop,Arg2Strop,Arg3Strop,Arg4Strop,State)>
        {
            typedef typename StropReturnType<const Arg1Strop>::type Arg1Type;
            typedef typename StropReturnType<const Arg2Strop>::type Arg2Type;
            typedef typename StropReturnType<const Arg3Strop>::type Arg3Type;
            typedef typename StropReturnType<const Arg4Strop>::type Arg4Type;
            typedef typename F::template result<F(Arg1Type,Arg2Type,Arg3Type,Arg4Type)>::type R; 
            typedef const boost::shared_ptr<Strop<R(Arg1Type,Arg2Type,Arg3Type,Arg4Type)> > type;         
        };
        
        template<typename F, 
        typename Arg1Strop, 
        typename Arg2Strop, 
        typename Arg3Strop, 
        typename Arg4Strop, 
        typename State>
        typename result<generic_func(F&,Arg1Strop,Arg2Strop,Arg3Strop,Arg4Strop,State)>::type
        operator()(const F& f,
                   const Arg1Strop arg1, 
                   const Arg2Strop arg2, 
                   const Arg3Strop arg3, 
                   const Arg4Strop arg4, 
                   State engine)
        {   
            std::cout << "generic_func" << std::endl;
            typedef result<generic_func(F&,Arg1Strop,Arg2Strop,Arg3Strop,Arg4Strop,State)> Result;        
            typedef Func4<F
            ,typename Result::Arg1Type
            ,typename Result::Arg2Type
            ,typename Result::Arg3Type
            ,typename Result::Arg4Type
            > FuncStropType; 
            
            boost::shared_ptr<FuncStropType> funcStropPtr(new FuncStropType()); 
            
            typename BaseType<Arg1Strop>::type::OutputStreamPtr arg1Stream(arg1->MakeOutputStream());
            typename BaseType<Arg2Strop>::type::OutputStreamPtr arg2Stream(arg2->MakeOutputStream());
            typename BaseType<Arg2Strop>::type::OutputStreamPtr arg3Stream(arg3->MakeOutputStream());
            typename BaseType<Arg2Strop>::type::OutputStreamPtr arg4Stream(arg4->MakeOutputStream());
            funcStropPtr->SetInputs(boost::fusion::make_vector(arg1Stream, arg2Stream, arg3Stream, arg4Stream));
            
            engine->AddVertexToGraph(funcStropPtr);
            engine->AddEdgeToGraph(arg1->Descriptor(), funcStropPtr->Descriptor(), arg1Stream);
            engine->AddEdgeToGraph(arg2->Descriptor(), funcStropPtr->Descriptor(), arg2Stream);
            engine->AddEdgeToGraph(arg3->Descriptor(), funcStropPtr->Descriptor(), arg3Stream);
            engine->AddEdgeToGraph(arg4->Descriptor(), funcStropPtr->Descriptor(), arg4Stream);
            return funcStropPtr;
        }
        
        // Arity = 5
        template<typename F, 
                 typename Arg1Strop, 
                 typename Arg2Strop, 
                 typename Arg3Strop, 
                 typename Arg4Strop, 
                 typename Arg5Strop, 
                 typename State>
        struct result<generic_func(F&,Arg1Strop,Arg2Strop,Arg3Strop,Arg4Strop,Arg5Strop,State)>
        {
            typedef typename StropReturnType<const Arg1Strop>::type Arg1Type;
            typedef typename StropReturnType<const Arg2Strop>::type Arg2Type;
            typedef typename StropReturnType<const Arg3Strop>::type Arg3Type;
            typedef typename StropReturnType<const Arg4Strop>::type Arg4Type;
            typedef typename StropReturnType<const Arg5Strop>::type Arg5Type;
            typedef typename F::template result<F(Arg1Type,Arg2Type,Arg3Type,Arg4Type,Arg5Type)>::type R; 
            typedef const boost::shared_ptr<Strop<R(Arg1Type,Arg2Type,Arg3Type,Arg4Type,Arg5Type)> > type;         
        };
        
        template<typename F, 
                 typename Arg1Strop, 
                 typename Arg2Strop, 
                 typename Arg3Strop, 
                 typename Arg4Strop, 
                 typename Arg5Strop, 
                 typename State>
        typename result<generic_func(F&,Arg1Strop,Arg2Strop,Arg3Strop,Arg4Strop,Arg5Strop,State)>::type
        operator()(const F& f,
                   const Arg1Strop arg1, 
                   const Arg2Strop arg2, 
                   const Arg3Strop arg3, 
                   const Arg4Strop arg4, 
                   const Arg5Strop arg5, 
                   State engine)
        {   
            std::cout << "generic_func" << std::endl;
            typedef result<generic_func(F&,Arg1Strop,Arg2Strop,Arg3Strop,Arg4Strop,Arg5Strop,State)> Result;        
            typedef Func5<F
                        ,typename Result::Arg1Type
                        ,typename Result::Arg2Type
                        ,typename Result::Arg3Type
                        ,typename Result::Arg4Type
                        ,typename Result::Arg5Type
                        > FuncStropType; 
            
            boost::shared_ptr<FuncStropType> funcStropPtr(new FuncStropType()); 
            
            typename BaseType<Arg1Strop>::type::OutputStreamPtr arg1Stream(arg1->MakeOutputStream());
            typename BaseType<Arg2Strop>::type::OutputStreamPtr arg2Stream(arg2->MakeOutputStream());
            typename BaseType<Arg3Strop>::type::OutputStreamPtr arg3Stream(arg3->MakeOutputStream());
            typename BaseType<Arg4Strop>::type::OutputStreamPtr arg4Stream(arg4->MakeOutputStream());
            typename BaseType<Arg5Strop>::type::OutputStreamPtr arg5Stream(arg5->MakeOutputStream());
            funcStropPtr->SetInputs(boost::fusion::make_vector(arg1Stream, arg2Stream, arg3Stream, arg4Stream, arg5Stream));
            
            engine->AddVertexToGraph(funcStropPtr);
            engine->AddEdgeToGraph(arg1->Descriptor(), funcStropPtr->Descriptor(), arg1Stream);
            engine->AddEdgeToGraph(arg2->Descriptor(), funcStropPtr->Descriptor(), arg2Stream);
            engine->AddEdgeToGraph(arg3->Descriptor(), funcStropPtr->Descriptor(), arg3Stream);
            engine->AddEdgeToGraph(arg4->Descriptor(), funcStropPtr->Descriptor(), arg4Stream);
            engine->AddEdgeToGraph(arg5->Descriptor(), funcStropPtr->Descriptor(), arg5Stream);
            return funcStropPtr;
        }

    };
    
    template<typename F, typename CallableDummy = proto::callable>
    struct lib_func : proto::callable
    {
        template<typename Sig> struct result;
        
        template<typename LhsStrop, typename RhsStrop, typename State>
        struct result<lib_func(LhsStrop,RhsStrop,State)> 
        : generic_func::result<generic_func(F&,LhsStrop,RhsStrop,State)> 
        {};    
        
        template<typename LhsStrop, typename RhsStrop, typename State>
        typename result<lib_func(LhsStrop,RhsStrop,State)>::type
        operator()(const LhsStrop left, const RhsStrop right, State engine)
        {
            generic_func gf;
            F func;
            std::cout << "builtin func: " << typeid(func).name() << std::endl;
            return gf(func,left,right,engine);
        }
    };
    
} // ns streamulus