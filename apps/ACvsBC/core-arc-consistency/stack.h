// Stack implementation
// Part of https://github.com/pothitos/naxos

#ifndef Ns_STACK_H
#define Ns_STACK_H

template <class TemplType>
class NsStack {

    protected:
        struct StackNode_t {
                TemplType theData;
                StackNode_t* next;
                StackNode_t(const TemplType& T) : theData(T), next(0)
                {
                }
        };

        StackNode_t* stackTopNode;

    public:
        typedef TemplType value_type;

        typedef unsigned long size_type;

    private:
        size_type nFrames;

    public:
        NsStack(void) : stackTopNode(0), nFrames(0)
        {
        }

        // NsStack (const NsStack& stackOther);

        NsStack(const NsStack& stackOther)
        {
                *this = stackOther;
        }

        NsStack& operator=(const NsStack& stackOther);

        ~NsStack(void)
        {
                clear();
        }

        size_type size(void) const
        {
                return nFrames;
        }

        void clear(void)
        {
                while (!empty())
                        pop();
        }

        bool empty(void) const
        {
                return (stackTopNode == 0);
        }

        TemplType& top(void)
        {
                assert_exc(stackTopNode != 0, "NsStack::top: Stack is empty");
                return stackTopNode->theData;
        }

        const TemplType& top(void) const
        {
                assert_exc(stackTopNode != 0, "NsStack::top: Stack is empty");
                return stackTopNode->theData;
        }

        // TemplType&  top_MinusOne (void)
        //{
        //    assert_exc( stackTopNode!=0 && stackTopNode->next!=0 ,
        //    "NsStack::top_MinusOne: There is no second item in the
        //    stackTopNode"); return  stackTopNode->next->theData;
        //}

        void pop(void)
        {
                assert_exc(!empty(), "NsStack::pop: Stack is empty");
                StackNode_t* current = stackTopNode;
                stackTopNode = current->next;
                delete current;
                --nFrames;
        }

        void push(const TemplType newData)
        {
                StackNode_t* newNode = new StackNode_t(newData);
                // newNode->theData = newData;
                newNode->next = stackTopNode;
                stackTopNode = newNode;
                ++nFrames;
        }

        // Declaration necessary so the following
        // 'friend' statement sees this 'iterator'
        // instead of std::iterator:
        class iterator;
        friend class iterator;

        class iterator {
            private:
                StackNode_t* currNode;

            public:
                iterator(void) : currNode(0)
                {
                }

                iterator(StackNode_t* startNode) : currNode(startNode)
                {
                }

                bool operator==(const iterator& b) const
                {
                        return (currNode == b.currNode);
                }

                bool operator!=(const iterator& b) const
                {
                        return (!(*this == b));
                }

                TemplType& operator*(void)const
                {
                        assert_exc(currNode != 0, "NsStack::iterator::*: Bad "
                                                  "request "
                                                  "`*(something.end())'");
                        return currNode->theData;
                }

                TemplType* operator->(void)const
                {
                        assert_exc(currNode != 0, "NsStack::iterator::->: Bad "
                                                  "request "
                                                  "`*(something.end())'");
                        return &currNode->theData;
                }

                iterator& end(void)
                {
                        currNode = 0;
                        return *this;
                }

                iterator& operator++(void)
                {
                        assert_exc(currNode != 0, "NsStack::iterator::++: Bad "
                                                  "request "
                                                  "`++(something.end())'");
                        currNode = currNode->next;
                        return *this;
                }
        };

        iterator begin(void)
        {
                return iterator(stackTopNode);
        }

        iterator end(void)
        {
                iterator iter_end(stackTopNode);
                return iter_end.end();
        }
};

// template <class TemplType>
// void  NsStack<TemplType>::clear (void)

// template <class TemplType>
// void  NsStack<TemplType>::pop (void)

// template <class TemplType>
// void  NsStack<TemplType>::push (const TemplType newData)

template <class TemplType>
NsStack<TemplType>& NsStack<TemplType>::operator=(const NsStack& stackOther)
{
        // assert_exc( nFrames == 0  &&  stackTopNode == 0 ,
        //	"NsStack::operator=: Non empty `*this'");
        nFrames = stackOther.nFrames;
        StackNode_t** currNode = &stackTopNode;
        StackNode_t* const* currNodeOther = &stackOther.stackTopNode;
        for (;;) {
                if (*currNodeOther == 0) {
                        *currNode = *currNodeOther;
                        break;
                }
                *currNode = new StackNode_t(**currNodeOther);
                currNode = &(*currNode)->next;
                currNodeOther = &(*currNodeOther)->next;
        }
        return *this;
}
#endif // Ns_STACK_H
