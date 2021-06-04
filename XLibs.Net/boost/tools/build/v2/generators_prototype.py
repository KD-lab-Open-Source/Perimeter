# Copyright David Abrahams 2003. Permission to copy, use,
# modify, sell and distribute this software is granted provided this
# copyright notice appears in all copies. This software is provided
# "as is" without express or implied warranty, and with no claim as
# to its suitability for any purpose.

# Importing by a different name keeps PyChecker happy
from __future__ import generators as generators_ 
import sys

    
class Generator(object):
    """Representation of a transformation from source to target types.
    
       sources and targets may be either strings or sequences of
       strings.

       >>> print Generator(('obj*', 'lib*'), 'exe')
       exe <- obj*,lib*

       >>> assert Generator('c','o').unary
    """
    
    def __init__(self, sources, targets):
        """
        >>> g = Generator(['obj*', 'z', 'cpp'], ['lib','dll'])
        >>> g.signature
        [('cpp', 1, 1), ('obj', 0, '*'), ('z', 1, 1)]
        >>> g = Generator('cpp', 'obj')
        >>> g.signature
        [('cpp', 1, 1)]
        """
        self.sources = _sequence_of_strings(sources)
        self.targets =_sequence_of_strings(targets)
        self.targets_ = _string_multiset(targets)

        signature = {}
        stars = {}
        for s in self.sources:
            if s.endswith('*'):
                stars[s[:-1]] = 1
                signature.setdefault(s[:-1],0)
            else:
                signature[s] = signature.get(s,0) + 1

        self.signature = []
        for t, n in signature.items():
            if t in stars:
                self.signature.append((t, n, '*'))
            else:
                self.signature.append((t, n, n))

        self.signature.sort() # makes doctests nicer
        
        # Remember whether the signature is strictly unary
        self.unary = not stars and len(self.sources) == 1

    def match(self, group):
        """If group satisfies an element of the signature, returns an
        amended signature that consists of all other elements.
        Otherwise, returns None.
        >>> g = Generator(['obj*', 'z', 'cpp', 'z'], ['lib','dll'])
        >>> g.match(TargetTypeGroup('obj',12))
        [('cpp', 1, 1), ('z', 2, 2)]
        >>> g.match(TargetTypeGroup('cpp',12)) # None
        
        >>> g.match(TargetTypeGroup('cpp',1))
        [('obj', 0, '*'), ('z', 2, 2)]

        >>> g.match(TargetTypeGroup('z',2))
        [('cpp', 1, 1), ('obj', 0, '*')]
        
        >>> Generator('cpp','obj').match(TargetTypeGroup('cpp',1))
        []

        >>> Generator('cpp','obj').match(TargetTypeGroup('cpp',12))
        []
        """
        if self in group.generators:
            return None
        
        for i in range(len(self.signature)):
            e = self.signature[i]
            if e[0] == group.target_type:
                if self.unary:
                    return []
                if e[1] > group.size or e[2] < group.size:
                    return None
                else:
                    return self.signature[:i] + self.signature[i+1:]
        return None

    def __str__(self):
        """Make a nice human-readable representation
        >>> g = Generator(['obj*', 'z', 'cpp'], ['lib','dll'])
        >>> print g
        lib,dll <- obj*,z,cpp
        """
        return ','.join(self.targets) + ' <- ' + ','.join(self.sources)

    def __type_list_rep(self, type_list):
        if len(type_list) == 1:
            return repr(type_list[0])
        else:
            return repr(type_list)
        
    def __repr__(self):
        return (
            self.__module__ + '.' + type(self).__name__ + '(' +
            self.__type_list_rep(self.sources)
            + ', ' + self.__type_list_rep(self.targets) + ')'
            )

def _dict_tuple(d):
    l = d.items()
    l.sort()
    return tuple(l)

def _sorted(list):
    list.sort()
    return list

class GeneratorSet(object):
    def __init__(self):
        self.all_generators = {}
        self.generators_by_type = {}
        
    def __iadd__(self, generator):
        """Add a generator to the set

        >>> s = GeneratorSet()
        >>> s += Generator('foo', 'bar')
        >>> s += Generator('foo', 'baz')
        >>> s += Generator(['foo','bar'], 'baz')
        >>> s += Generator('bar', ['baz', 'mumble'])
        >>> s += Generator('bar*', ['bing'])
        >>> print s
        {
          bar:
            baz <- foo,bar
            baz,mumble <- bar
            bing <- bar*
          foo:
            bar <- foo
            baz <- foo
            baz <- foo,bar
        }

        """
        if not generator in self.all_generators:
            self.all_generators[generator] = 1
            for t in generator.sources:
                if t.endswith('*'):
                    t = t[:-1]
                l = self[t]
                l.append(generator)
        return self

    def __isub__(self, generator):
        for t in generator.sources:
            if t.endswith('*'):
                t = t[:-1]
            self[t].remove(generator)
        return self
    
    def __getitem__(self, t):
        """Given a target type, return a list of all the generators
        which can consume that target type.
        """
        return self.generators_by_type.setdefault(t,[])

    def __str__(self):
        # import pprint
        # return pprint.pformat(self.generators_by_type)
    
        s = []
        
        for k,v in _sorted(self.generators_by_type.items()):
            s += [ '  ' + k + ':\n    ' + '\n    '.join([ str(x) for x in v ]) ]

        return '{\n' + '\n'.join(s) + '\n}'

def _dicts_intersect(d1, d2):
    """True iff d1 and d2 have a key in common

    >>> assert _dicts_intersect({1:0, 2:0}, {2:0})
    >>> assert _dicts_intersect({2:0}, {1:0, 2:0})
    >>> assert not _dicts_intersect({1:0, 3:0}, {2:0})
    >>> assert not _dicts_intersect({2:0}, {1:0, 3:0})
    """
    if len(d2) < len(d1):
        tmp = d1
        d1 = d2
        d2 = tmp
    for k in d1.iterkeys():
        if k in d2:
            return True
    return False
    
class TargetTypeGroup(object):
    instances = 0
    
    def __init__(
        self
        , target_type
        , size       # how many of that type are in the group.
        , parents = ()
        , generator = None):
        """
        >>> g1 = TargetTypeGroup('x', 1)
        >>> assert not g1.extra_targets
        >>> assert g1.consumed_sources == { g1:1 }
        >>> g2 = TargetTypeGroup('x', 1)
        
        >>> g3 = TargetTypeGroup('x', 1, [g1,g2])
        >>> assert g1 in g3.consumed_sources
        >>> assert g2 in g3.consumed_sources
        >>> assert not g3 in g3.consumed_sources
        """
        self.target_type = target_type
        self.size = size
        self.parents = parents
        self.generator = generator
        self.siblings = None
        self.id = TargetTypeGroup.instances
        self.ambiguous = reduce(lambda x,y: x or y.ambiguous and 1,
                                parents, None)

        self.generators = { generator : 1 } # it doesn't hurt to store None here
        ignored = [ self.generators.update(p.generators) for p in parents ]
        
        self.__constituents = None
        self.__extra_targets = None
        
        TargetTypeGroup.instances += 1

        if generator:
            self.moves = { (id(generator),id(parents)) : 1 }
        else:
            self.moves = {}
        
        if not parents:
            self.consumed_sources = {self:1}
            self.__extra_targets = ()
        else:
            ignored = [ self.moves.update(p.moves) for p in parents ]
            
            if len(parents) == 1:
                self.consumed_sources = parents[0].consumed_sources
            else:
                self.consumed_sources = {}
                for c in parents:
                    self.consumed_sources.update(c.consumed_sources)

    # constituents property - the set of all target groups consumed in
    # creating this group
    def __get_constituents(self):
        if self.__constituents is None:
            self.__constituents = {self:1}
            for c in self.parents:
                self.__constituents.update(c.constituents)
        return self.__constituents
            
    constituents = property(__get_constituents)

    cost = property(lambda self: len(self.moves))
    
    # extra targets property - in general, every target group sits at
    # the root of a DAG.  The extra targets are the ones produced by
    # generators that run in this DAG but which are not part of the
    # DAG, i.e. are not constituents.  In the example below, X and Y
    # are extra targets of A.
    #
    #           A   X       B,C <- D
    #          / \ /        C,Y <- E
    #         B   C   Y     A,X <- B,C
    #          \ / \ /      
    # Sources:  D   E
    #
    # We use the extra targets to determine the equivalence of two
    # search States
    def __get_extra_targets(self):
        if self.__extra_targets is None:
            
            if len(self.parents) == 1 and not self.siblings:
                self.__extra_targets = self.parents[0].extra_targets
            else:
                # all siblings are created incidentally
                if self.siblings:
                    t = tuple([s for s in self.siblings if s != self])
                else:
                    t = ()
                    
                # Any groups created incidentally as part of generating my
                # parents are also incidental to my generation
                for c in self.parents:
                    for i in c.extra_targets:
                        if i not in self.constituents:
                            t += (i,)

                self.__extra_targets = _sort_tuple(t)
        return self.__extra_targets

    extra_targets = property(__get_extra_targets)
    
    def set_siblings(self, sibs):
        assert self.__extra_targets is None, \
               "can't set siblings after extra targets already computed."

        assert self.parents, "original source nodes don't have siblings"
        self.siblings = sibs

    def __repr__(self):
        return '%s.%s(#%s$%s)' % (self.size,self.target_type,self.id,self.cost)
    
    def is_compatible_with(self, other):
        """True iff self and other can be used to trigger a generator.
        
        >>> g1 = TargetTypeGroup('x', 1)
        >>> g2 = TargetTypeGroup('x', 1)
        >>> assert g1.is_compatible_with(g2)
        
        >>> g3 = TargetTypeGroup('x', 1, [g1])
        >>> g4 = TargetTypeGroup('x', 1, [g2])
        >>> assert g3.is_compatible_with(g4)
        >>> assert g3.is_compatible_with(g2)
        >>> assert not g3.is_compatible_with(g1)
        >>> assert not g2.is_compatible_with(g4)

        >>> g5 = TargetTypeGroup('x', 1, [g3])
        >>> assert not g5.is_compatible_with(g1)
        """
        return not _dicts_intersect(
            self.constituents, other.constituents)

    def all_compatible(self, others):
        """True iff self is_compatible with every element of other
        """
        for o in others:
            if not self.is_compatible_with(o):
                return False
        return True

    def atoms(self):
        """If this group was formed by combining other groups without
        a generator, return a set of its nearest parent groups which
        were not formed that way.  Otherwise, return a set
        containing only this group.

        >>> g1 = TargetTypeGroup('x',1)
        >>> g2 = TargetTypeGroup('x',1)
        >>> a = TargetTypeGroup('x',2, [g1,g2]).atoms()
        >>> assert g1 in a and g2 in a and len(a) == 2
        """
        if self.generator or not self.parents:
            return (self,)
        x = ()
        for p in self.parents:
            x += p.atoms()
        return x

    
    def consumes(self, others):
        """True iff not self is_compatible with every element of other
        """
        for o in others:
            if self.is_compatible_with(o):
                return False
        return True
    
def _string_multiset(s):
    x = {}
    for t in _sequence_of_strings(s):
        x[t] = x.get(t,0) + 1
    return x


def parent_sets(chosen, signature, all_groups, generator):
    """Given an already-chosen tuple of TargetTypeGroups and a signature
    of the groups left to choose, generates all mutually-compatible
    combinations of groups starting with chosen

    >>> TargetTypeGroup.instances = 0
    >>> groups = {
    ...    'x': [ TargetTypeGroup('x', 1) ],
    ...    'y': [ TargetTypeGroup('y', 1), TargetTypeGroup('y',2) ],
    ...    'z': [ TargetTypeGroup('z', 1) ]
    ... }
    >>> signature = (('y',0,'*'),('z',1,'1'))
    >>> chosen = (groups['x'][0],)
    >>> [ x for x in parent_sets(chosen, signature, groups, Generator('x',('y*', 'z'))) ]
    [(1.x(#0$0), 1.z(#3$0)), (1.x(#0$0), 1.y(#1$0), 1.z(#3$0)), (1.x(#0$0), 2.y(#2$0), 1.z(#3$0))]
    """
    if len(signature) == 0:
        # The entire signature was satisfied; we can just yield the
        # one result
        yield chosen
    else:
        # find all ways to satisfy the next element of the signature
        # which are compatible with the already-chosen groups.  If
        # there are no ways, we will fall off the end here and the
        # ultimate result will be empty.
        t, min, max = signature[0]
        
        if min == 0:
            for s in parent_sets(chosen, signature[1:], all_groups, generator):
                yield s
            
        for g in all_groups[t]:

            # can only use a generator once in any path
            if generator in g.generators:
                continue
            
            if (g.size >= min and g.size <= max and
                g.all_compatible(chosen)): 
                
                for s in parent_sets(
                    chosen + (g,), signature[1:], all_groups, generator
                    ):
                    yield s
        
debug = None

def _sort_tuple(t):
    """copies the given sequence into a new tuple in sorted order"""
    l = list(t)
    l.sort()
    return tuple(l)

def _sequence_of_strings(s_or_l):
    """if the argument is a string, wraps a tuple around it.
    Otherwise, returns the argument untouched
    """
    if isinstance(s_or_l, type('')):
        return (s_or_l,)
    else:
        return s_or_l

def _examine_edge(states, queue, g):
    """Handle a possible new state in the search.
    """
    g_state = State(g)
    v = states.setdefault(g_state, g_state)

    if v.group is g:
        queue.append(g_state)
        return False

    if v.group.cost > g.cost:
        if debug:
            print 'reducing cost of state(%s) via %s' % (v.group,g)
        v.group.ambiguous = None
        v.group = g

    elif v.group.cost < g.cost:
        if debug:
            print 'discarding %s due to lower cost state(%s)' % (g, v.group)

    elif not (g.generator or v.group.generator) \
             and _sort_tuple(g.atoms()) == _sort_tuple(v.group.atoms()):
        # These are two different ways of combining the same groups of
        # a given type to produce a larger group, without using a generator
        if debug:
            print 'discarding %s as a redundant formulation of %s' % (g,v.group)
    else:
        if debug:
            print '%s is an ambiguous path due to %s' % (v.group, g)
        # Remember the group which caused the ambiguity
        v.group.ambiguous = g

    return True

class State(object):
    """A wrapper around a TargetTypeGroup which makes it hashable on
    the part of its data which determines its ability to contribute to
    producing the goal target type.    
    """
    def __init__(self, group):
        self.group = group
        
    def __hash__(self):
        g = self.group
        x = g.consumed_sources.keys()
        x.sort()
        return hash((g.target_type, g.size, _sort_tuple(g.extra_targets), tuple(x)))

    def __eq__(self, other):
        return (
            self.group.target_type == other.group.target_type
            and self.group.size == other.group.size
            and self.group.extra_targets == other.group.extra_targets
            and self.group.consumed_sources == other.group.consumed_sources)

queue_moves = 0

def optimal_graphs(target_type, source_groups, generators):
    """A 'simple generator' that produces the sequence of least-cost
    solutions for producing the
    target type from a subset of the source_groups, using the given
    generators.
    """
    # An index from target type to lists of groups with that type.
    all_groups = {}
    
    # Prime the priority Queue
    q = [ State(g) for g in source_groups ]
    
    # Keep a record of all known states in the search
    states = dict([ (s,s) for s in q ])

    solution_cost = None
    while q:
        # remove a group from the queue
        g = q[0].group
        del q[0]
        
        global queue_moves
        queue_moves += 1

        global debug
        if debug:
            print '-------'
            print graph(g)
        
        if g.target_type == target_type: # and g.consumes(source_groups):
            solution_cost = g.cost
            yield g
            
            if g.consumes(source_groups): # Nothing left to find
                return
        
        # combine with all like groups which are compatible
        for g2 in all_groups.get(g.target_type,()):

            if g2.is_compatible_with(g):
                
                _examine_edge(
                    states, q,
                    TargetTypeGroup(g.target_type, g2.size + g.size, (g,g2)))
        
        # expand with all generators which can be triggered as a
        # result of adding this group
        for generator in generators[g.target_type]:

            match = generator.match(g)
            if match is None:
                continue

            if debug:
                print generator,' matched with ', match
            
            # for all sets of parents which match the generator and
            # include g
            for s in parent_sets((g,), match, all_groups, generator):

                # Create the products of running this generator with
                # the given parent set
                siblings = ()
                for t,n in generator.targets_.items():
                    # Unary generators run as many times as neccessary
                    # to consume the group
                    if (generator.unary):
                        n *= g.size
                    
                    siblings += (TargetTypeGroup(t, n, s, generator),)

                # Make sure groups know about their siblings
                if len(siblings) > 1:
                    for product in siblings:
                        product.set_siblings(siblings)
                        
                if debug:
                    print siblings, '<-', list(s)
                    
                # Add new search states to the queue
                for sib in siblings:
                    _examine_edge(states, q, sib)

        # Add to the set of all groups so that we can combine it with
        # others in future iterations
        l = all_groups.get(g.target_type)
        if l is None:
            l = all_groups.setdefault(g.target_type,[])
        l.append(g)

        # Sort the queue; in 'real life' use a priority queue
        q.sort(lambda v1,v2: v1.group.cost - v2.group.cost)
        
                    
def graph(group, indent = 0, visited = None):
    
    """Produce a string representation of the search graph
    that produced the given group.
    """
    if (visited is None):
        visited = {}
    s = indent * '    '
    s += repr(group)
    if group in visited:
        s += '...\n'
    else:
        visited[group] = True
        s += '[%s]' % group.generator

        if group.ambiguous:
            s += ' *ambiguous* '
            if type(group.ambiguous) is not type(1):
                s += 'due to %s' % group.ambiguous
        if group.siblings:
            s += ' siblings ' + str([sib for sib in group.siblings if
                                     sib != group])
        s += '\n' + '\n'.join(
            [graph(g,indent+1,visited) for g in group.parents])
    return s

def ambiguities(group):
    """Returns a list of groups that caused ambiguities with this one
    or its constituents.
    """
    result = []
    for g in group.parents:
        result.extend(ambiguities(g))
    if group.ambiguous and type(group.ambiguous) is not type(1):
        result.append(group.ambiguous)
    return result

def search(generators, targets, sources):
    import sys
    global queue_moves
    
    TargetTypeGroup.instances = 0
    queue_moves = 0

    # Remember what we started with
    source_groups = tuple([
        TargetTypeGroup(i[0],i[1])
        for i in _string_multiset(sources).items() ])

    solutions = {}
    max_consumed = 0
    
    for g in optimal_graphs(targets, source_groups, generators):

        if len(g.consumed_sources) > max_consumed:
            max_consumed = len(g.consumed_sources)
            
        g2 = solutions.setdefault(len(g.consumed_sources), g)
        if g2 is not g:
            if g2.cost == g.cost:
                g2.ambiguous = g

    if max_consumed:
        g = solutions[max_consumed]
        
        print 80 * '='
        print graph(g)

        if g.ambiguous:
            print 40 * '-'
            print 'ambiguities:'
            for a in ambiguities(g):
                print graph(a)
                print
                
        print queue_moves, 'queue_moves'
        print 80 * '='
        sys.stdout.flush()

    print queue_moves, 'queue moves'
    print '\n\n*****\n\n'

    
def test():
    """Runs Volodya's example, but doesn't get the result he'd like.
    """
    # EST_EXE <- OBJ*
    # OBJ <- CPP
    # {CPP,STATIC_DATA} <- NM_ASM
    # {CPP,CPP} <- ECPP (only first CPP must be further converted into NM_ASM)
    # NM_ASM <- CPP
    # {CPP,STATIC_DATA} <- STATIC_DATA*
    # STATIC_DATA <- NM_ASM
    # NM_OBJ <- NM_ASM
    # NM_EXE <- NM_OBJ*
    generators = GeneratorSet()
    generators += Generator('OBJ*', 'EST_EXE')
    generators += Generator('CPP', 'OBJ')
    generators += Generator('NM_ASM', ('CPP', 'STATIC_DATA'))
    generators += Generator('ECPP', ('CPP','CPP'))
    generators += Generator('CPP', 'NM_ASM')
    generators += Generator('STATIC_DATA*', ('CPP', 'STATIC_DATA'))
    generators += Generator('NM_ASM', 'NM_OBJ')
    generators += Generator('NM_OBJ*', 'NM_EXE')

    search(generators, 'EST_EXE', ('CPP', 'NM_ASM', 'ECPP'))
    # Try the same search with a source type that can't be consumed.
    # This will exhaust all transformations before stopping.
    search(generators, 'EST_EXE', ('CPP', 'NM_ASM', 'ECPP', 'FOO'))
    search(generators, 'NM_EXE', ('CPP'))

def run(args = None):
    import doctest
    import sys

    if args is not None:
        sys.argv = args
        
    error = doctest.testmod(sys.modules.get(__name__))
    
    if not error[0]:
        global debug
        if '--debug' in sys.argv:
            debug = 1
        test()
        
    return error
    
if __name__ == '__main__':
    import sys
    sys.exit(run()[0])
        











