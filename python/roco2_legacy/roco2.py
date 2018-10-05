import csv
from fnmatch import fnmatch

import numpy as np

from . import Kernel


class Experiment(object):
    reserved_fields = ['begin', 'end', 'experiment', 'thread_count', 'frequency', 'ddcm', 'utility']

    def __init__(self, begin, end, experiment, thread_count, frequency, ddcm, utility, **kwargs):
        self.kernel = Kernel(int(experiment))
        self.thread_count = int(thread_count)
        self.frequency = int(frequency)
        self.ddcm = float(ddcm)

        self.utility = float(utility)
        self._values = {channel: float(value) for channel, value in kwargs.items()}

    def __getitem__(self, chan_or_pat):
        if chan_or_pat in self._values:
            return self._values[chan_or_pat]
        try:
            return getattr(self, chan_or_pat)
        except AttributeError:
            pass
        sum = 0.
        count = 0
        for channel, value in self._values.items():
            if fnmatch(channel, chan_or_pat):
                sum += value
                count += 1
        if count == 0:
            raise ValueError("No channel with pattern {} found.".format(chan_or_pat))
        return sum

    @property
    def name(self):
        return self.kernel.name

    def __str__(self):
        return "Experiment {} with {} threads @ {} MHz".format(self.name, self.threads, self.frequency / 1e6)


class ExperimentList(object):
    def __init__(self, experiments, filter_attrs={}, filter_functions=[]):
        self._experiments = experiments
        self._filter_attrs = filter_attrs
        self._filter_functions = filter_functions

    def __iter__(self):
        for ex in self._experiments:
            if all(getattr(ex, key) == value for key, value in self._filter_attrs.items()) and \
                    all(ff(ex) for ff in self._filter_functions):
                yield ex

    def filter(self, filter_function=None, **kwargs):
        new_ffs = list(self._filter_functions)
        if filter_function is not None:
            new_ffs.append(filter_function)
        new_fas = dict(**self._filter_attrs, **kwargs)
        return ExperimentList(self._experiments, new_fas, new_ffs)

    def reset_filter(self, **kwargs):
        return ExperimentList(self._experiments).filter(**kwargs)

    @property
    def filters(self):
        return self._filter_attrs

    # TODO Implement version that can take multiple groups
    def group(self, group):
        keys = sorted(set(ex[group] for ex in self))
        for key in keys:
            yield key, self.filter(**{group: key})

    def values(self, *patterns):
        if len(patterns) == 1:
            return np.array([ex[patterns[0]] for ex in self])
        elif len(patterns) > 1:
            return tuple(self.values(pattern) for pattern in patterns)
        else:
            raise TypeError("values expected at least 2 argument")

    @property
    def kernels(self):
        return sorted(set(ex.kernel for ex in self), key=lambda x: x.value)

    @property
    def thread_counts(self):
        return sorted(set(ex.thread_count for ex in self), reverse=True)

    @property
    def frequencies(self):
        return sorted(set(ex.frequency for ex in self), reverse=True)

    @property
    def ddcms(self):
        return sorted(set(ex.ddcm for ex in self), reversed=True)


class FileExperimentList(ExperimentList):
    def __init__(self, csv_file, delimiter='\t'):
        try:
            reader = csv.DictReader(csv_file, delimiter=delimiter, quotechar='\'')
            experiments = [Experiment(**row) for row in reader]
            super().__init__(experiments)
        finally:
            csv_file.close()
