////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputPort.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    //
    // InputPortBase
    //
    template <typename ValueType>
    InputPortBase::InputPortBase(const class Node* owningNode, const PortElements<ValueType>& inputs, std::string name, size_t dimension) : Port(owningNode, name, Port::GetPortType<ValueType>(), dimension), _inputElements(inputs)
    {
    }

    template <typename ValueType>
    std::vector<ValueType> InputPortBase::GetTypedValue() const
    {
        std::vector<ValueType> result;
        size_t size = Size();
        result.reserve(size);
        for(size_t index = 0; index < size; ++index)
        {
            auto element = _inputElements.GetElement(index);
            auto typedOutput = static_cast<const OutputPort<ValueType>*>(element.ReferencedPort());
            auto temp = typedOutput->GetOutput(element.GetIndex());
            result.push_back(temp);
        }

        if (Size() != result.size())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
        return result;
    }

    template <typename ValueType>
    ValueType InputPortBase::GetTypedValue(size_t index) const
    {        
        const auto& element = _inputElements.GetElement(index);
        auto typedOutput = static_cast<const OutputPort<ValueType>*>(element.ReferencedPort());
        return typedOutput->GetOutput(element.GetIndex());
    }

    inline void InputPortBase::ComputeParentsAndElements()
    {
        for (const auto& range : _inputElements.GetRanges())
        {
            auto port = range.ReferencedPort();
            auto node = port->GetNode();
            _parentNodes.push_back(node);
        }
    }

    //
    // InputPort
    //
    template <typename ValueType>
    InputPort<ValueType>::InputPort(const class Node* owningNode, const PortElements<ValueType>& input, std::string name) : InputPortBase(owningNode, _input, name, input.Size()), _input(input)
    {
        ComputeParentsAndElements();
    }

    template <typename ValueType>
    ValueType InputPort<ValueType>::GetValue(size_t index) const
    {
        return GetTypedValue<ValueType>(index);
    }

    template <typename ValueType>
    ValueType InputPort<ValueType>::operator[](size_t index) const
    {
        return GetTypedValue<ValueType>(index);
    }

    template <typename ValueType>
    PortElements<ValueType> InputPort<ValueType>::GetPortElements() const
    {
        return _input;
    }

    template <typename ValueType>
    void InputPort<ValueType>::Serialize(utilities::Serializer& serializer) const
    {
        InputPortBase::Serialize(serializer);
        serializer.Serialize("inputElements", _input);
    }

    template <typename ValueType>
    void InputPort<ValueType>::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        InputPortBase::Deserialize(serializer, context);
        serializer.Deserialize("inputElements", _input, context);

        ComputeParentsAndElements();
    }
}
